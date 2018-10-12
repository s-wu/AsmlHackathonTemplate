from flask import Flask, render_template, request, jsonify
app = Flask(__name__)

@app.route('/')
def index():
    return render_template("home.html")


@app.route('/save', methods=['GET', 'POST'])
def save():
    if request.method == 'POST':
        print(request.get_json())
        resp = jsonify(success=True)
        return resp

if __name__ == '__main__':
    app.run(debug=True)
