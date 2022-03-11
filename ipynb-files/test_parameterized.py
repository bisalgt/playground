import pytest

@pytest.mark.parametrize("x,y,z", [(12,13,14),(15,16,17)])
def test_method(x,y,z):
    assert x+y == z