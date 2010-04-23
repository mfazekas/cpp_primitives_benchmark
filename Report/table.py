#!/usr/bin/python

class Table(object):
    def __init__(self):
        self.header = []
        self.body = []
    def to_html(self,res):
        res.write('<table>')
        res.write('<tr>')
        for h in self.header:
            res.write('<th>')
            res.write(str(h))
            res.write('</th>')
        res.write('</tr>')
        res.write("\n")
        for r in self.body:
            res.write('<tr>') 
            for c in r:
                res.write('<td>') 
                res.write(str(c))
                res.write('</td>')
            res.write('</tr>')
            res.write("\n")
        res.write('</table>')
        res.write("\n")
    def to_twiki(self,res):
        res.write("|*")
        res.write("*|*".join([str(h) for h in self.header]))
        res.write("*|\n")
        for r in self.body:
            res.write("|")
            res.write("|".join([str(i) for i in r]))
            res.write("|\n")
    def to_format(self,format,res):
        formats = {
            'twiki':self.to_twiki,
            'html':self.to_html }
        if not formats.has_key(format):
            raise Exception("Unsupported table format:%s supported formats:%s" % (format,formats.keys))
        formats[format](res)

import unittest
from cStringIO import StringIO

class TableTestFormats(unittest.TestCase):
    def setUp(self):
        self.t = Table()
        self.t.header = ['a','b']
        self.t.body = [[1,2],[2,3]]
    def testAsHtml(self):
        res = StringIO()
        self.t.to_html(res)
        self.assertEqual("<table><tr><th>a</th><th>b</th></tr>\n<tr><td>1</td><td>2</td></tr>\n<tr><td>2</td><td>3</td></tr>\n</table>\n",res.getvalue())
    def testAsTwiki(self):
        res = StringIO()
        self.t.to_twiki(res)
        self.assertEqual("|*a*|*b*|\n|1|2|\n|2|3|\n",res.getvalue())
    def testToFormat(self):
        res1,res2 = StringIO(),StringIO()
        self.t.to_twiki(res1)
        self.t.to_format('twiki',res2)
        self.assertEqual(res1.getvalue(),res2.getvalue())
        res1,res2 = StringIO(),StringIO()
        self.t.to_html(res1)
        self.t.to_format('html',res2)
        self.assertEqual(res1.getvalue(),res2.getvalue())

if __name__=="__main__":
    unittest.main()
