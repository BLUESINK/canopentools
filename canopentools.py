import argparse
from genslave import genslave
from gendoc import gendoc

if __name__ == '__main__' :
	parser = argparse.ArgumentParser(description='BLUESINK CANOpen Tools')
	subparsers = parser.add_subparsers(dest='cmd', required=True, help='sub-commands')

	parser_a = subparsers.add_parser('genslave', help='CANOpen Slave Stack Code generator')
	parser_a.add_argument('--show', action='store_true', help='Show Object Dictionary Tree')
	parser_a.add_argument('EDS_FILE', type=str, help='Location of CANOpen EDS File')
	parser_a.add_argument('OUTPUT_PATH', type=str, help='Destination folder path of CANOpen Slave Stack Code')

	parser_b = subparsers.add_parser('genmaster', help='CANOpen Master Stack Code generator')
	parser_b.add_argument("--opt3", action='store_true')
	parser_b.add_argument("--opt4", action='store_true')

	parser_c = subparsers.add_parser('gendoc', help='Document generator based on EDS file')
	parser_c.add_argument('--show', action='store_true', help='Show Object Dictionary Tree')
	parser_c.add_argument('EDS_FILE', type=str, help='Location of CANOpen EDS File')
	parser_c.add_argument('DESC_PATH', type=str, help='Location of Description Folder')
	parser_c.add_argument('OUTPUT_PATH', type=str, help='Destination folder path of Document')

	args = parser.parse_args()

	if args.cmd == 'genslave' :
		genslave.generate(args.EDS_FILE, args.OUTPUT_PATH, args.show)
	if args.cmd == 'gendoc' :
		gendoc.generate(args.EDS_FILE, args.DESC_PATH, args.OUTPUT_PATH, args.show)