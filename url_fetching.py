import requests

# The URL you want to fetch
url = "https://www.w3schools.com/html/html_basic.asp"

# Send HTTP GET request
response = requests.get(url)

# Check if request was successful
if response.status_code == 200:
    html_content = response.text
    
    # Save HTML to a file
    with open("output.html", "w", encoding="utf-8") as f:
        f.write(html_content)
    
    print("HTML saved to output.html")
else:
    print(f"Failed to fetch page. Status code: {response.status_code}")