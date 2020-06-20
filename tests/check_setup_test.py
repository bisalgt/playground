import unittest

class CheckStringMethods(unittest.TestCase):
    
    def setUp(self):
        print('Iniside test fixture: setup')
        self.title = 'this is the title'
    
    def test_lower(self):
        print('Iniside test case: test_lower')
        self.assertEqual(self.title.lower(), 'this is the title')
    
    def test_upper(self):
        print('Inside test case: test upper')
        self.assertEqual(self.title.upper(), 'THIS IS THE TITLE')
    
    def test_title(self):
        print('Inside test case: test title')
        self.assertEqual(self.title.title(), 'THis is the title')


    def tearDown(self):
        print('Inside test fixutre: tearDown')
        del self.title

if __name__ == '__main__':
    unittest.main()

