python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

python3 -m grpc_tools.protoc --proto_path=.. --python_out=. --grpc_python_out=. ../service.proto 
