import pytest
import requests
import json

@pytest.fixture
def main_url():
    return "https://reqres.in"

def test_login_valid(main_url):
    url = main_url + "/api/login/"
    data = {"email": "abc@xyh.com", "password": "12344dff"}
    response = requests.post(url, data= data)
    token = json.loads(response.text)
    assert response.status_code == 200
    assert token["token"] == "QpwL5tke4Pnpja7X4"
    
def test_login_valid2(main_url):
    url = main_url + "/api/login/"
    data = {"email": "eve.holt@reqres.in", "password": "cityslicka"}
    response = requests.post(url, data= data)
    token = json.loads(response.text)
    assert response.status_code == 200
    assert token["token"] == "QpwL5tke4Pnpja7X4"

# since no error will be generated, it will act as pass
def test_check():
    pass