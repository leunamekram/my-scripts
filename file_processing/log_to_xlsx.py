# coding=utf-8
"""
Simple script for parsing IAR watch log file to excel file
"""

from xlsxwriter import Workbook
from os import startfile

fileSfx = '_160_p30_p32'
logFile = 'Watch3{:s}.log'.format(fileSfx)

excelFile = 'antenna_diversity.xlsx'

ANTENNA_DIV_ARRAY_SZ = 19
ANTENNA_DIV_STRUCT_SZ = 4
ANTENNA_DIV_PERCENT_DIV = 50

antennaDivFailedRx = []
antennaDivPreambleRssi = []
antennaDivPreambleMatch = []

logArrays = {'antennaDivFailedRx' : antennaDivFailedRx,
             'antennaDivPreambleRssi' : antennaDivPreambleRssi,
             'antennaDivPreambleMatch' : antennaDivPreambleMatch}


def parseLogFile():
    '''
    Read and extract data from log file
    '''
    print('\nParsing log file...')

    with open(logFile) as hFile:
        logFileContent = hFile.readlines()
        logFileContent = logFileContent[1:]

        # Read and parse file
        for line in logFileContent:
            # Change line to text list
            line = line.split()

            # Parse lines
            if line[0] in logArrays.keys():
                arrayName = line[0]
                print('Reading data log for {:s} ...'.format(arrayName))

            elif line[1] == '<array>':
                logDataCount = 0
                logDataList = []
            
            elif line[1] == '<struct>':
                logDataStruct = []

            # Store data values
            else:
                logData = int(line[1])
                logDataCount = logDataCount + 1

                # Parsing of data for preamble match array is done differently due to struct.
                if arrayName == 'antennaDivPreambleMatch':
                    logDataStruct.append(logData)

                    # Store data to struct
                    if (logDataCount % ANTENNA_DIV_STRUCT_SZ) == 0:
                        logDataList.append(logDataStruct)

                    # Store struct to data list
                    if logDataCount == (ANTENNA_DIV_ARRAY_SZ * ANTENNA_DIV_STRUCT_SZ):
                        logArrays[arrayName].append(logDataList)

                else:
                    logDataList.append(logData)

                    # Store data to list
                    if logDataCount == ANTENNA_DIV_ARRAY_SZ:
                        logArrays[arrayName].append(logDataList)

    hFile.close()


def createXlsxFile():
    '''
    Create excel workbook
    '''
    print('\nCreating excel file...')

    workbook = Workbook(excelFile)

    # Fill spreadsheet with logfile contents
    for arrayName, logArrayList in sorted(logArrays.items()):
        worksheet = workbook.add_worksheet(arrayName)

        print('Creating worksheet for {:s}...'.format(arrayName))
        row = 0
        col = 0

        for logArray in logArrayList:
            for logData in logArray:
                if arrayName == 'antennaDivPreambleMatch':
                    worksheet.write(row, col, logData[0]/ANTENNA_DIV_PERCENT_DIV)
                    worksheet.write(row + 22, col, logData[1]/ANTENNA_DIV_PERCENT_DIV)
                    worksheet.write(row + 44, col, logData[2]/ANTENNA_DIV_PERCENT_DIV)
                    worksheet.write(row + 66, col, logData[3]/ANTENNA_DIV_PERCENT_DIV)

                elif arrayName == 'antennaDivFailedRx':
                    # Compute for error rate of failed RX                    
                    worksheet.write(row, col, logData/ANTENNA_DIV_PERCENT_DIV)

                else:
                    worksheet.write(row, col, logData)

                row = row + 1

            row = 0
            col = col + 1

    workbook.close()


def main():
    print('\nRunning in script mode...')

    parseLogFile()
    createXlsxFile()

    # Launch excel file on windows
    print('\nOpening excel file...\n')
    startfile(excelFile)


if __name__ == '__main__':
    main()
