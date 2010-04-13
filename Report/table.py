#!/usr/bin/python
# Copyright (c) 2007- Miklos Fazekas
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#    This product includes software developed by the <organization>.
# 4. Neither the name of the <organization> nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
