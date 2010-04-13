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
