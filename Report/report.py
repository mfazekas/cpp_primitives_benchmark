"""
Usage: report.py primitive-operations-test-exec

"""

from optparse import OptionParser
import subprocess
import sys

from table import Table
from report_to_table import ReportToTable

def parse_args():
    parser = OptionParser()
    parser.add_option("-t", "--testtool", dest="testtool",
                  help="specifiy the PATH of the performance tool",metavar="PATH")
    parser.add_option("-o", "--output", dest="output",
                  help="sepecify PATH to dump the output, defaults to stdout",metavar="PATH",default="/dev/stdout")
    parser.add_option("-f", "--format", dest="format",
                  help="specify FORMAT to be used to print the table",metavar="FORMAT",default="html")
    (options, args) = parser.parse_args()
    if len(args) > 0:
        parser.error("unexpected agrguments:%s" % args)
    if not options.testtool:
        parser.error("--testtool argument is required")
    return options

def main():
    options = parse_args()
    out = subprocess.Popen(options.testtool,stdout=subprocess.PIPE).stdout
    table = Table()
    reporter = ReportToTable()
    reporter.process_xml(out,table)
    output = None
    if options.output == '/dev/stdout':
        output = sys.stdout
    else:
        output = open(options.output,'w')
    table.to_format(options.format,output)

main()
