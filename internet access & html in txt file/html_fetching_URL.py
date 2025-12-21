import sys
import requests

# Usage: python html_fetching_URL.py <url> <output_file>
default_url = "https://www.w3schools.com/html/html_basic.asp"
default_output = "output.html"

url = sys.argv[1] if len(sys.argv) > 1 else default_url
output_file = sys.argv[2] if len(sys.argv) > 2 else default_output

response = requests.get(url)

if response.status_code == 200:
    html_content = response.text
    with open(output_file, "w", encoding="utf-8") as f:
        f.write(html_content)
    print(f"HTML saved to {output_file}")
else:
    print(f"Failed to fetch page. Status code: {response.status_code}")
