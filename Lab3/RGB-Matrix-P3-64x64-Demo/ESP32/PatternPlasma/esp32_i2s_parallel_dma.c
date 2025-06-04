/*
 * ESP32_I2S_PARALLEL_DMA (Version 3) - ESP32-S2/S3 Compatible
 * 
 * Author:      Mrfaptastic @ https://github.com/mrfaptastic/
 * 
 * Description: Multi-ESP32 product DMA setup functions for WROOM & S2, S3 mcu's.
 *
 * Credits:
 *  1) https://www.esp32.com/viewtopic.php?f=17&t=3188          for original ref. implementation 
 *  2) https://github.com/TobleMiner/esp_i2s_parallel           for a cleaner implementation
 *
 */

// Header
#include "esp32_i2s_parallel_dma.h" 
#include "esp32_i2s_parallel_mcu_def.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <driver/gpio.h>
#include <driver/periph_ctrl.h>
#include <soc/gpio_sig_map.h>

// Handle different ESP32 variants
#ifdef CONFIG_IDF_TARGET_ESP32
    #include <soc/i2s_struct.h>
    #define ESP32_ORIG
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    #include <soc/i2s_struct.h>
    #define ESP32_SXXX
#elif defined(CONFIG_IDF_TARGET_ESP32S3)  
    #include <soc/i2s_struct.h>
    #define ESP32_SXXX
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #include <soc/i2s_struct.h>
    #define ESP32_SXXX
#else
    #include <soc/i2s_struct.h>
    #define ESP32_ORIG  // Default fallback
#endif

// For I2S state management.
static i2s_parallel_state_t *i2s_state = NULL;

// ESP32-S2,S3,C3 only has I2S0
// Original ESP32 has two I2S's, but we'll stick with the lowest common denominator.
#ifdef ESP32_ORIG
static i2s_dev_t* I2S[I2S_NUM_MAX] = {&I2S0, &I2S1};
#else
static i2s_dev_t* I2S[I2S_NUM_MAX] = {&I2S0};	
#endif

callback shiftCompleteCallback = NULL;
void setShiftCompleteCallback(callback f) {
    shiftCompleteCallback = f;
}

volatile int  previousBufferOutputLoopCount = 0;
volatile bool previousBufferFree = true;

static void IRAM_ATTR irq_hndlr(void* arg) { 
    // Clear the interrupt flag - use proper register for ESP32-S2/S3
#ifdef ESP32_ORIG
    SET_PERI_REG_BITS(I2S_INT_CLR_REG(ESP32_I2S_DEVICE), I2S_OUT_EOF_INT_CLR_V, 1, I2S_OUT_EOF_INT_CLR_S);
#else
    // For ESP32-S2/S3
    I2S0.int_clr.out_eof = 1;
#endif

    previousBufferFree = true;

    // Call callback if defined
    if(shiftCompleteCallback) {
        shiftCompleteCallback();
    }
}

// For peripheral setup and configuration
static inline int get_bus_width(i2s_parallel_cfg_bits_t width) {
    switch(width) {
        case I2S_PARALLEL_WIDTH_8:
            return 8;
        case I2S_PARALLEL_WIDTH_16:
            return 16;
        case I2S_PARALLEL_WIDTH_24:
            return 24;
        default:
            return -ESP_ERR_INVALID_ARG;
    }
}

static void iomux_set_signal(int gpio, int signal) {
    if(gpio < 0) {
        return;
    }
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
    gpio_set_direction(gpio, GPIO_MODE_DEF_OUTPUT);
    gpio_matrix_out(gpio, signal, false, false);
    
    // More mA the better...
    gpio_set_drive_capability((gpio_num_t)gpio, (gpio_drive_cap_t)3);
}

static void dma_reset(i2s_dev_t* dev) {
    dev->lc_conf.in_rst = 1;
    dev->lc_conf.in_rst = 0;
    dev->lc_conf.out_rst = 1;
    dev->lc_conf.out_rst = 0;
    
    dev->lc_conf.ahbm_rst = 1;
    dev->lc_conf.ahbm_rst = 0;
}

static void fifo_reset(i2s_dev_t* dev) {
    dev->conf.rx_fifo_reset = 1;
    
#ifdef ESP32_SXXX
    while(dev->conf.rx_fifo_reset_st); // esp32-s2/s3 only
#endif
    dev->conf.rx_fifo_reset = 0;
    
    dev->conf.tx_fifo_reset = 1;
#ifdef ESP32_SXXX
    while(dev->conf.tx_fifo_reset_st); // esp32-s2/s3 only
#endif

    dev->conf.tx_fifo_reset = 0;
}

static void dev_reset(i2s_dev_t* dev) {
    fifo_reset(dev);
    dma_reset(dev);
    dev->conf.rx_reset = 1;
    dev->conf.tx_reset = 1;
    dev->conf.rx_reset = 0;
    dev->conf.tx_reset = 0;  
}

// DMA Linked List
void link_dma_desc(volatile lldesc_t *dmadesc, volatile lldesc_t *prevdmadesc, void *memory, size_t size) {
    if(size > DMA_MAX) size = DMA_MAX;

    dmadesc->size = size;
    dmadesc->length = size;
    dmadesc->buf = memory;
    dmadesc->eof = 0;
    dmadesc->sosf = 0;
    dmadesc->owner = 1;
    dmadesc->qe.stqe_next = 0;
    dmadesc->offset = 0;

    // link previous to current
    if(prevdmadesc) {
        prevdmadesc->qe.stqe_next = (lldesc_t*)dmadesc;
    }
}

esp_err_t i2s_parallel_driver_install(i2s_port_t port, i2s_parallel_config_t* conf) {
    // Validate parameters
    if(port < I2S_NUM_0 || port >= I2S_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if(conf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if(conf->sample_width < I2S_PARALLEL_WIDTH_8 || conf->sample_width >= I2S_PARALLEL_WIDTH_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if(conf->sample_rate > I2S_PARALLEL_CLOCK_HZ || conf->sample_rate < 1) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint32_t clk_div_main = I2S_PARALLEL_CLOCK_HZ / conf->sample_rate / i2s_parallel_get_memory_width(port, conf->sample_width);
    if(clk_div_main < 2 || clk_div_main > 0xFF) {
        return ESP_ERR_INVALID_ARG;
    }

    volatile int iomux_signal_base;
    volatile int iomux_clock;
    int irq_source;
    
    // Initialize I2S0 peripheral
    if (port == I2S_NUM_0) {
#ifdef ESP32_ORIG        
        periph_module_reset(PERIPH_I2S0_MODULE);
        periph_module_enable(PERIPH_I2S0_MODULE);
#else
        // ESP32-S2/S3 peripheral control
        periph_module_reset(PERIPH_I2S0_MODULE);
        periph_module_enable(PERIPH_I2S0_MODULE);
#endif
        iomux_clock = I2S0O_WS_OUT_IDX;
        irq_source = ETS_I2S0_INTR_SOURCE;

        switch(conf->sample_width) {
            case I2S_PARALLEL_WIDTH_8:
            case I2S_PARALLEL_WIDTH_16:
                iomux_signal_base = I2S0O_DATA_OUT8_IDX;
                break;
            case I2S_PARALLEL_WIDTH_24:
                iomux_signal_base = I2S0O_DATA_OUT0_IDX;
                break;
            case I2S_PARALLEL_WIDTH_MAX:
                return ESP_ERR_INVALID_ARG;
        }
    } 
#ifdef ESP32_ORIG    
    else if (port == I2S_NUM_1) {
        periph_module_reset(PERIPH_I2S1_MODULE);
        periph_module_enable(PERIPH_I2S1_MODULE);
        iomux_clock = I2S1O_WS_OUT_IDX;
        irq_source = ETS_I2S1_INTR_SOURCE;

        switch(conf->sample_width) {
            case I2S_PARALLEL_WIDTH_16:
                iomux_signal_base = I2S1O_DATA_OUT8_IDX;
                break;
            case I2S_PARALLEL_WIDTH_8:
            case I2S_PARALLEL_WIDTH_24:
                iomux_signal_base = I2S1O_DATA_OUT0_IDX;
                break;
            case I2S_PARALLEL_WIDTH_MAX:
                return ESP_ERR_INVALID_ARG;
        }
    } 
#endif
    else {
        return ESP_ERR_INVALID_ARG;
    }

    // Setup GPIOs
    int bus_width = get_bus_width(conf->sample_width);
    if(bus_width < 0) {
        return bus_width;
    }

    // Setup I2S peripheral
    i2s_dev_t* dev = I2S[port];
    
    // Setup GPIO's
    for(int i = 0; i < bus_width; i++) {
        iomux_set_signal(conf->gpio_bus[i], iomux_signal_base + i);
    }
    iomux_set_signal(conf->gpio_clk, iomux_clock);
    
    // invert clock phase if required
    if (conf->clkphase) {
        GPIO.func_out_sel_cfg[conf->gpio_clk].inv_sel = 1;
    }

    // Setup i2s clock
    dev->sample_rate_conf.val = 0;
    
    // Third stage config, width of data to be written to IO
    dev->sample_rate_conf.rx_bits_mod = bus_width;
    dev->sample_rate_conf.tx_bits_mod = bus_width;
    
    dev->sample_rate_conf.rx_bck_div_num = 2;
    dev->sample_rate_conf.tx_bck_div_num = 2;
    
    // Clock configuration
    dev->clkm_conf.val = 0;
    
#ifdef ESP32_SXXX
    dev->clkm_conf.clk_sel = 2; // esp32-s2/s3 only  
    dev->clkm_conf.clk_en = 1;
#endif

#ifdef ESP32_ORIG
    dev->clkm_conf.clka_en = 0;
#endif
    
    dev->clkm_conf.clkm_div_b = 0;
    dev->clkm_conf.clkm_div_a = 1;
    dev->clkm_conf.clkm_div_num = clk_div_main;

    // I2S conf2 reg
    dev->conf2.val = 0;
    dev->conf2.lcd_en = 1;
    dev->conf2.lcd_tx_wrx2_en = 0; 
    dev->conf2.lcd_tx_sdx2_en = 0;    

    // I2S conf reg
    dev->conf.val = 0;   
    
#ifdef ESP32_SXXX  
    dev->conf.tx_dma_equal = 1;
    dev->conf.pre_req_en = 1;
#endif

    // DMA FIFO setup
    dev->fifo_conf.val = 0;  
    dev->fifo_conf.rx_data_num = 32;
    dev->fifo_conf.tx_data_num = 32;  
    dev->fifo_conf.dscr_en = 1;  

#ifdef ESP32_ORIG
    if(conf->sample_width == I2S_PARALLEL_WIDTH_8) {
        dev->conf2.lcd_tx_wrx2_en = 1;  
    }

    if(conf->sample_width == I2S_PARALLEL_WIDTH_24) {
        dev->fifo_conf.tx_fifo_mod = 3;
    } else {
        dev->fifo_conf.tx_fifo_mod = 1;
    }
    
    dev->fifo_conf.rx_fifo_mod_force_en = 1;
    dev->fifo_conf.tx_fifo_mod_force_en = 1;
    
    dev->conf_chan.val = 0;
    dev->conf_chan.tx_chan_mod = 1;
    dev->conf_chan.rx_chan_mod = 1;
#endif  

    // Device Reset
    dev_reset(dev);    
    dev->conf1.val = 0;
    dev->conf1.tx_stop_en = 0; 
    
    // Allocate I2S status structure
    if(i2s_state != NULL) {
        free(i2s_state);
    }
    
    i2s_state = (i2s_parallel_state_t*) malloc(sizeof(i2s_parallel_state_t));
    if(i2s_state == NULL) {
        return ESP_ERR_NO_MEM;
    }
    
    i2s_parallel_state_t *state = i2s_state;
    state->desccount_a = conf->desccount_a;
    state->desccount_b = conf->desccount_b;
    state->dmadesc_a = conf->lldesc_a;
    state->dmadesc_b = conf->lldesc_b;  
    state->i2s_interrupt_port_arg = port;

    dev->timing.val = 0;

    // Setup interrupt if requested
    if (conf->int_ena_out_eof) {
        esp_err_t err = esp_intr_alloc(irq_source, 
                                     (int)(ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL1),
                                     irq_hndlr, 
                                     &state->i2s_interrupt_port_arg, NULL);
        
        if(err) {
            free(i2s_state);
            i2s_state = NULL;
            return err;
        }

        dev->int_ena.out_eof = 1;
    }
   
    return ESP_OK;
}

esp_err_t i2s_parallel_stop_dma(i2s_port_t port) {
    if(port < I2S_NUM_0 || port >= I2S_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    i2s_dev_t* dev = I2S[port];
    
    // Stop DMA operations
    dev->out_link.stop = 1;
    dev->out_link.start = 0;
    dev->conf.tx_start = 0;
    
    return ESP_OK;
}

esp_err_t i2s_parallel_send_dma(i2s_port_t port, lldesc_t* dma_descriptor) {
    if(port < I2S_NUM_0 || port >= I2S_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if(dma_descriptor == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2s_dev_t* dev = I2S[port];

    // Configure DMA burst mode
    dev->lc_conf.val = I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN;

    // Set DMA descriptor address
    dev->out_link.addr = (uint32_t) dma_descriptor;
 
    // Start DMA operation
    dev->out_link.stop = 0; 
    dev->out_link.start = 1;
    dev->conf.tx_start = 1;

    return ESP_OK;
}

// Double buffering functions
void i2s_parallel_flip_to_buffer(i2s_port_t port, int buffer_id) {
    if (i2s_state == NULL) {
        return;
    }

    lldesc_t *active_dma_chain;
    if (buffer_id == 0) {
        active_dma_chain = (lldesc_t*)&i2s_state->dmadesc_a[0];
    } else {
        active_dma_chain = (lldesc_t*)&i2s_state->dmadesc_b[0];
    }

    if(i2s_state->desccount_a > 0) {
        i2s_state->dmadesc_a[i2s_state->desccount_a-1].qe.stqe_next = active_dma_chain;
    }
    if(i2s_state->desccount_b > 0) {
        i2s_state->dmadesc_b[i2s_state->desccount_b-1].qe.stqe_next = active_dma_chain;
    }

    i2s_parallel_set_previous_buffer_not_free();
}

bool i2s_parallel_is_previous_buffer_free() {
    return previousBufferFree;
}

void i2s_parallel_set_previous_buffer_not_free() {
    previousBufferFree = false;
    previousBufferOutputLoopCount = 0;
}

// Cleanup function
esp_err_t i2s_parallel_driver_uninstall(i2s_port_t port) {
    if(port < I2S_NUM_0 || port >= I2S_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    
    i2s_parallel_stop_dma(port);
    
    i2s_dev_t* dev = I2S[port];
    dev->int_ena.out_eof = 0;
    
    if(i2s_state != NULL) {
        free(i2s_state);
        i2s_state = NULL;
    }
    
    if(port == I2S_NUM_0) {
        periph_module_disable(PERIPH_I2S0_MODULE);
    }
#ifdef ESP32_ORIG
    else if(port == I2S_NUM_1) {
        periph_module_disable(PERIPH_I2S1_MODULE);
    }
#endif
    
    return ESP_OK;
}