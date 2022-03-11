
import pytest


class TestClass:
    def test_one(self):
        x = 111
        assert x == 111
    
    @pytest.mark.skip
    def test_two(self):
        x = 123
        assert x == 111
    
    
# whenever we have to run something before each test, we use PYTEST FIXTURE as below

# Fixtures are not meant to be called directly,
# but are created automatically when test functions request them as parameters.
# so in our case, we need to provide numbers as a parameter for our test_functions.




@pytest.fixture
def numbers():
    a = 10
    b = 12
    c = 15
    return [a, b, c]

@pytest.mark.xfail
def test_method11(numbers):
    x = 13
    assert x == numbers[0]

@pytest.mark.xfail
def test_method12(numbers):
    x = 12
    assert x == numbers[0]

def test_method13(numbers):
    x = 10
    assert x == numbers[0]