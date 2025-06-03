# 🤖 Camp Navigator – AI Assistant for Artemis DSAI 2025
# ---------------------------------------------------
# This chatbot answers your questions using content pulled directly
# from the camp GitHub repo (README, schedule, FSM code, etc).
# It follows the MCP pattern: See → Think → Do → Speak → Loop

# 📦 STEP 1: Install required libraries
!pip install openai scikit-learn --quiet

# ---------------------------------------------------
# 🔐 STEP 2: Add your OpenAI API key
# (This key will be used to access ChatGPT)
import openai

# 🔑 Replace with your OpenAI key or let students paste theirs here
openai.api_key = "sk-..."  # ← Insert your API key here

# ---------------------------------------------------
# 📁 STEP 3: Load ALL files from GitHub repo (RudyMartin/dsai-2025)
# This uses GitHub API to recursively fetch all text-based files
import requests

REPO = "RudyMartin/dsai-2025"
BRANCH = "main"

# GitHub API URL for listing repo contents recursively
api_url = f"https://api.github.com/repos/{REPO}/git/trees/{BRANCH}?recursive=1"

# Fetch list of all files in the repo
tree_resp = requests.get(api_url)
tree_data = tree_resp.json()

# Filter only .md and .ino files (add more extensions if needed)
file_paths = [
    item["path"]
    for item in tree_data.get("tree", [])
    if item["path"].endswith((".md", ".ino", ".txt")) and item["type"] == "blob"
]

print(f"📂 Found {len(file_paths)} files to load...")

# Base raw URL
base_raw_url = f"https://raw.githubusercontent.com/{REPO}/{BRANCH}/"

# Download all file contents
docs = []
for path in file_paths:
    raw_url = base_raw_url + path
    r = requests.get(raw_url)
    if r.status_code == 200:
        docs.append(r.text)
        print(f"✅ Loaded: {path}")
    else:
        print(f"❌ Failed: {path}")

# Combine all content into a single text blob
full_text = "\n\n".join(docs)

# ---------------------------------------------------
# 🔍 STEP 4: Chunk the content and create a retriever
# We'll use a simple TF-IDF vectorizer for text similarity
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

# Split by paragraph blocks
chunks = full_text.split("\n\n")
vectorizer = TfidfVectorizer().fit_transform(chunks)

# ---------------------------------------------------
# 💬 STEP 5: Define the Camp Navigator Query Function
# This function finds relevant info and sends a prompt to ChatGPT
def ask_navigator(query):
    # SEE: Vectorize the query
    q_vec = TfidfVectorizer().fit(chunks).transform([query])

    # THINK: Find most similar text chunk
    scores = cosine_similarity(q_vec, vectorizer).flatten()
    top_idx = scores.argmax()
    context = chunks[top_idx]

    # DO: Use the top chunk as context for GPT prompt
    prompt = f"Use the following camp information to answer the question:\n\n{context}\n\nQ: {query}\nA:"

    # SPEAK: Call OpenAI to get a response
    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",
        messages=[{"role": "user", "content": prompt}]
    )

    # LOOP: Display result and wait for next question
    print("🧠 FSM / MCP Trace → See → Think → Do → Speak → Loop\n")
    print("🔍 Top Match from Camp Docs:\n", context[:300], "...\n")
    print("🤖 Answer:\n", response["choices"][0]["message"]["content"])

# ---------------------------------------------------
# ▶️ STEP 6: Try it!
# Ask about the schedule, a lab, or a concept
ask_navigator("What happens in Lab 3?")
