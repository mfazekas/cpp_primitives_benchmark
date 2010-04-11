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
"""
Usage: report.py primitive-operations-test-exec

"""

import subprocess
import xml.etree.ElementTree as ET
import itertools

print(__doc__+"\n")
test_program = '/Work/MyProjects/PrimitveOperationsPerfTest/build/Release/PrimitveOperationsPerfTest'

def dump_element(element):
    name = element.findtext('unit/name')
    amount = element.findtext('unit/amount')
    time = element.findtext('time')
    print element

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
    

class ReportToTable(object):
    unit_info = {'operations':
                    {'columns':
                       [{'name':'op time',
                         'value':lambda report:ReportToTable._get_column(report,'operations'),
                         'normalize':lambda value:ReportToTable._normalize(value,'seconds')},
                        {'name':'ops per sec',
                         'value':lambda report:ReportToTable._get_persec_column(report,'operations'),
                         'normalize':lambda value:ReportToTable._normalize(value)}
                       ]
                    }
                }
    @classmethod
    def _formatfloat(klass,value):
        roundedval = round(value,2)
        if roundedval == float(int(value)):
            return str(int(value))
        else:
            return str(roundedval)
    @classmethod
    def _normalize(klass,value,unit=None):
        if unit == None:
            return klass._formatfloat(value)
        else:
            unitscale = [{'name':'s','scale':1},
                         {'name':'ms','scale':1000},
                         {'name':'us','scale':1000*1000},
                         {'name':'ns','scale':1000*1000*1000}]
            scaletouse = None
            for scale in unitscale:
                if not scaletouse and value > 1/float(scale['scale']):
                    scaletouse = scale
            if not scaletouse:
                scaletouse = unitscale[-1]
            return '%s %s' % (klass._formatfloat(value*float(scaletouse['scale'])),scaletouse['name'])
    @classmethod
    def _get_persec_column(klass,report,name,dispunit=None):
        for unit in report.getiterator('unit'):
            if unit.find('name').text == name:
                res = float(unit.find('amount').text)/float(report.find('time').text)
                return res
    @classmethod
    def _get_column(klass,report,name,dispunit=None):
        for unit in report.getiterator('unit'):
            if unit.find('name').text == name:
                res = float(report.find('time').text)/float(unit.find('amount').text)
                return res
    def create_header(self,tree):
        self.column_info = [{'name':'name','value':lambda report:report.find('name').text,'normalize':lambda x:x}]
        for i in tree.getiterator('report'):
            for unit in i.getiterator('unit'):
                ET.dump(unit)
                unit_name = unit.find('name').text
                for column in self.unit_info[unit_name]['columns']:
                    if not column in self.column_info:
                        self.column_info += [column]
        return [column['name'] for column in self.column_info]
    def create_row(self,element):
        result = []
        for c in self.column_info:
            result += [c['value'](element)]
        
        return result
    def normalize_row(self,row):
        result = []
        for (r,c) in itertools.izip(row,self.column_info):
            result += [c['normalize'](r)]
        return result
    def _sort(self,columnidx,rows):
        rows.sort(cmp=lambda a,b: cmp(a[columnidx],b[columnidx]))
    def process_xml(self,xmlinput):
        tree = ET.ElementTree()
        tree.parse(xmlinput)
        result = Table()
        result.header = self.create_header(tree)
        for report in tree.getiterator('report'):
            result.body += [self.create_row(report)]
        self._sort(1,result.body)
        rows = result.body
        result.body = []
        for row in rows:
            result.body += [self.normalize_row(row)]
        return result

def main():
    import sys
    out = subprocess.Popen(test_program,stdout=subprocess.PIPE).stdout
    ret = ReportToTable()
    table = ret.process_xml(out)
    table.to_html(sys.stdout)

import unittest
from cStringIO import StringIO

class TableTest(unittest.TestCase):
    def testAsHtml(self):
        t = Table()
        t.header = ['a','b']
        t.body = [[1,2],[2,3]]
        res = StringIO()
        t.to_html(res)
        self.assertEqual("<table><tr><th>a</th><th>b</th></tr>\n<tr><td>1</td><td>2</td></tr>\n<tr><td>2</td><td>3</td></tr>\n</table>\n",res.getvalue())
        

class ProcessXMLTest(unittest.TestCase):
    def testNormalize(self):
        self.assertEqual('333.33 ms',ReportToTable._normalize(1.0/3,'s'))
        self.assertEqual('0.33',ReportToTable._normalize(1.0/3))
        self.assertEqual('5',ReportToTable._normalize(5))
    def testCreateHeader(self):
        root = ET.Element('reports')
        report = ET.SubElement(root,'report')
        unit = ET.SubElement(report,'unit')
        ET.SubElement(unit,'name').text = 'operations'
        self.assertEqual(['name','op time','ops per sec'],ReportToTable().create_header(root))
    def testCreateHeaderShallNotDupicateHeaders(self):
        root = ET.Element('reports')
        report = ET.SubElement(root,'report')
        unit = ET.SubElement(report,'unit')
        ET.SubElement(unit,'name').text = 'operations'
        report2 = ET.SubElement(root,'report')
        unit2 = ET.SubElement(report2,'unit')
        ET.SubElement(unit2,'name').text = 'operations'
        self.assertEqual(['name','op time','ops per sec'],ReportToTable().create_header(root))
    def testCreteRow(self):
        root = ET.Element('reports')
        report = ET.SubElement(root,'report')
        unit = ET.SubElement(report,'unit')
        ET.SubElement(unit,'name').text = 'operations'
        ET.SubElement(unit,'amount').text = '100'
        ET.SubElement(report,'time').text = '1'
        ET.SubElement(report,'name').text = 'foo'
        reportprocessor = ReportToTable()
        reportprocessor.create_header(root)
        row = reportprocessor.create_row(report)
        row = reportprocessor.normalize_row(row)
        self.assertEqual(['foo','10 ms','100'],row)
    def testReport(self):
        xml = """<reports><report><unit><name>operations</name><amount>100</amount></unit><time>1</time><name>foo</name></report></reports>"""
        ret = ReportToTable().process_xml(StringIO(xml))
        self.assertEqual(ret.header,['name','op time','ops per sec'])
        self.assertEqual(ret.body,[['foo','10 ms','100']])
    def testReportSortedByNonNormalizedRows(self):
        xml = """<reports>
        <report><unit><name>operations</name><amount>100</amount></unit><time>1</time><name>foo</name></report>
        <report><unit><name>operations</name><amount>100</amount></unit><time>1000</time><name>foo2</name></report>
        <report><unit><name>operations</name><amount>200</amount></unit><time>1</time><name>foo3</name></report>
        </reports>"""
        ret = ReportToTable().process_xml(StringIO(xml))
        self.assertEqual(ret.header,['name','op time','ops per sec'])
        self.assertEqual(ret.body,[['foo3','5 ms','200'],['foo','10 ms','100'],['foo2','10 s','0.1']])

if __name__=="__main__":
    #unittest.main()
    main()
