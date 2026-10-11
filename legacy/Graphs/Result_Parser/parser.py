from pydoc import classname
from unicodedata import name
from lxml import etree

class arm(object):
    def __init__(self, classname, name, time):
        self.classname = classname
        self.name = name
        self.time = time
    
    def __str__(self):
        return ("classname = ")

source_doc = etree.parse("itf-results.xml")
for elem in source_doc.iter("testcase"):
    print(elem.tag, elem.getchildren)

