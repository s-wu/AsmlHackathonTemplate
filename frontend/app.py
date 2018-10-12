from flask import Flask, render_template, request, jsonify
app = Flask(__name__)
import requests


@app.route('/')
def index():
    return render_template("home.html")


@app.route('/save', methods=['GET', 'POST'])
def save():
    if request.method == 'POST':
        res = request.get_json()
        output = ""
        if 'res1' in res:
            for x in res.get("res1", []):
                for i in range(len(x)):
                    for j in range(len(x[0])):
                        output += str(x[i][j])
                    output += "\n"
            output += ";"
        else:
            output = res.get("res2", "")
        print(output)
        # requests.post()
        resp = jsonify(success=True)
        return resp

if __name__ == '__main__':
    app.run(debug=True)
