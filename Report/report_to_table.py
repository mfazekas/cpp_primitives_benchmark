#!/usr/bin/python

import xml.etree.ElementTree as ET
import itertools

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
    def process_xml(self,xmlinput,result):
        tree = ET.ElementTree()
        tree.parse(xmlinput)
        result.header = self.create_header(tree)
        for report in tree.getiterator('report'):
            result.body += [self.create_row(report)]
        self._sort(1,result.body)
        rows = result.body
        result.body = []
        for row in rows:
            result.body += [self.normalize_row(row)]
        return result

import unittest

from table import Table
from cStringIO import StringIO

class ReportToTableTest(unittest.TestCase):
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
        table = Table()
        ReportToTable().process_xml(StringIO(xml),table)
        self.assertEqual(table.header,['name','op time','ops per sec'])
        self.assertEqual(table.body,[['foo','10 ms','100']])
    def testReportSortedByNonNormalizedRows(self):
        xml = """<reports>
        <report><unit><name>operations</name><amount>100</amount></unit><time>1</time><name>foo</name></report>
        <report><unit><name>operations</name><amount>100</amount></unit><time>1000</time><name>foo2</name></report>
        <report><unit><name>operations</name><amount>200</amount></unit><time>1</time><name>foo3</name></report>
        </reports>"""
        table = Table()
        ReportToTable().process_xml(StringIO(xml),table)
        self.assertEqual(table.header,['name','op time','ops per sec'])
        self.assertEqual(table.body,[['foo3','5 ms','200'],['foo','10 ms','100'],['foo2','10 s','0.1']])

if __name__=="__main__":
    unittest.main()
