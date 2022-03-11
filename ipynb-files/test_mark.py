import pytest

@pytest.mark.one
def test_method0():
    x = 5
    y = 5
    assert x == y

@pytest.mark.one
def test_method1():
    x = 5
    y = 6
    assert x == y
    
@pytest.mark.two
def test_method2():
    a = 7
    b = 14
    assert a*2 == b