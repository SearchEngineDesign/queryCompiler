import os
from flask import Flask
import subprocess
from time import sleep
app = Flask(__name__)

@app.route('/')
def run_cpp_program():
    try:
        print("Running subprocess.")
        result = subprocess.Popen(
            ['./search', "./log/frontier/list", "./log/frontier/bloomfilter.bin"], 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE, 
            text=True
        )

        oldtext = ""
        linesRead = 0
        os.set_blocking(result.stdout.fileno(), False)

        while True:
            sleep(10)
            try:
                text = result.stdout.read()
                if text == oldtext:
                    print("Program stalled. exiting...")
                    result.kill()
                    return 1
                linesRead += len([n for n in text.splitlines() if "http" in n])
            except:
                print("Failed reading output.")
                result.kill()
                return 1
            oldtext = text
            print(" | " + str(linesRead) + " sites indexed.")
            if result.poll() is not None:
                break
        print(f"Process finished with exit code: {result.returncode}")
    except subprocess.CalledProcessError:
        print("Error executing subprocess.")
        return 1
    except FileNotFoundError:
        print("Error: subprocess file not found")
        return 1
    except Exception as e:
        print(f"Unknown error: {e}")
        return 1
    return 0

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=int(os.environ.get("PORT", 8080)))
