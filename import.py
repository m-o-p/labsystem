from zipfile import ZipFile

import yaml

import sys
import re
import os


class ImportError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def loadXML(sourceZip, path):
    data = sourceZip.read(path).decode('utf-8')

    def findTag(tag):
        result = re.search('<' + tag + '>(.*)</' + tag + '>', data, re.DOTALL)

        if result is None:
            return result

        if result.group(1) == "[HTML]\n":
            print(data)

        return result.group(1)

    return {
        'title': findTag('title'),
        'contents': findTag('contents'),
        'question': findTag('question'),
        'answerArray': findTag('answerArray'),
        'correctMask': findTag('correctMask'),
        'idx': findTag('idx'),
        'answerExplanation': findTag('answerExplanation'),
        'prelabCollectionIdx': findTag('prelabCollectionIdx'),
        'labCollectionIdx': findTag('labCollectionIdx'),
        'exampleSolution': findTag('exampleSolution'),
        'answerArray': findTag('answerArray'),
        'answerExplanation': findTag('answerExplanation'),
        'correctMask': findTag('correctMask'),
        'raw': data
    }


def parseDisplay(contents):
    data = re.search('\\[([a-zA-Z]+)\\](.*)', contents, re.DOTALL)

    if data:
        return (data.group(1), data.group(2))

    return ('Text', contents)


def findElementById(sourceZip, type, id):
    if type == 'm' or type == 'i':
        regex = re.compile(".*/data/withSolutions/" + type + ('{0:07d}'.format(id)) + ".txt")

        return [x for x in sourceZip.namelist() if regex.match(x)][0]

    regex = re.compile(".*/data/" + type + ('{0:07d}'.format(id)) + ".txt")

    return [x for x in sourceZip.namelist() if regex.match(x)][0]


def processChildren(sourceZip, targetZip, childrenString):
    children = []

    for element in childrenString.split(' '):
        data = re.search('([a-z])([0-9]+)', element)
        idx = int(data.group(2))
        elementType = data.group(1)
        elementPath = findElementById(sourceZip, elementType, idx)

        children.append(processElement(sourceZip, targetZip, elementType, elementPath))

    ret = []
    for childArray in children:
        ret.append(childArray[0])

        for child in childArray[1:]:
            child['indirect'] = True

            ret.append(child)

    return ret


def saveElement(targetZip, element):
    data = yaml.dump(element['meta'])

    targetZip.writestr(element['path'] + '.meta', data.encode())

    if 'data' in element:
        targetZip.writestr(element['path'] + '.data', element['data'].encode())

    if 'secret' in element:
        secretPath = element['path'] + '.meta'
        secretPath = re.sub('content/', 'secret/', secretPath)
        targetZip.writestr(secretPath, yaml.dump(element['secret']).encode())


def processCollection(sourceZip, targetZip, elementPath):
    source = loadXML(sourceZip, elementPath)

    children = processChildren(sourceZip, targetZip, source['contents'])
    childrenPaths = [child['path'] for child in children if 'indirect' not in child]

    return [{
        'path': source['title'],
        'meta': {
            'type': 'Collection',
            'children': childrenPaths
        },
        'children': children
    }]


def processDisplay(sourceZip, targetZip, elementPath):
    source = loadXML(sourceZip, elementPath)

    (displayType, contents) = parseDisplay(source['contents'])

    return [{
        'path': source['title'],
        'meta': {
            'type': 'Display',
            'displayType': displayType
        },
        'data': contents
    }]


def processMC(sourceZip, targetZip, elementPath):
    source = loadXML(sourceZip, elementPath)

    (displayType, contents) = parseDisplay(source['question'])

    correctMask = int(source['correctMask'])

    correct = [True if b == '1' else False for b in '{:>32b}'.format(correctMask)]

    answerArray = source['answerArray']

    questions = []

    regex = re.compile('<arrayElement>(.*?)</arrayElement>', re.DOTALL)
    for match in regex.finditer(answerArray):
        question = match.group(1)
        questions.append(question)

    correct = correct[32 - len(questions):32]

    elements = [{
        'path': source['idx'],
        'meta': {
            'type': 'Question',
            'questionType': 'MultipleChoice',
            'maxAllowedAnswers': 3,
            'maxAllowedMistakes': 0,
            'shuffle': True,
            'shuffleHints': True,
            'singleChoice': False,
            'optionCount': len(questions)
        },
        'secret': {
            'credits': 1,
            'roundHintCount': 0,
            'options': correct
        }
    }, {
        'path': source['idx'] + '-Display',
        'meta': {
            'type': 'Display',
            'displayType': displayType
        },
        'data': contents
    }
    ]

    for id, question in enumerate(questions):
        (displayType, contents) = parseDisplay(question)

        elements.append({
            'path': source['idx'] + '-Option-' + str(id),
            'meta': {
                'type': 'Display',
                'displayType': displayType
            },
            'data': contents
        })

        elements.append({
            'path': source['idx'] + '-Option-Correct-' + str(id),
            'meta': {
                'type': 'Display',
                'displayType': 'Text'
            },
            'data': ''
        })

        elements.append({
            'path': source['idx'] + '-Option-Hint-' + str(id),
            'meta': {
                'type': 'Display',
                'displayType': 'Text'
            },
            'data': ''
        })

    return elements


def processQuestion(sourceZip, targetZip, elementPath):
    source = loadXML(sourceZip, elementPath)

    (displayTypeQ, contentsQ) = parseDisplay(source['question'])
    (displayTypeH, contentsH) = parseDisplay(source['exampleSolution'])

    return [{
        'path': source['idx'],
        'meta': {
            'type': 'Question',
            'questionType': 'Text',
            'sectionCount': 0
        },
        'secret': {
            'credits': 1,
            'sections': 0
        }
    }, {
        'path': source['idx'] + '-Display',
        'meta': {
            'type': 'Display',
            'displayType': displayTypeQ
        },
        'data': contentsQ
    }, {
        'path': source['idx'] + '-Hint',
        'meta': {
            'type': 'Display',
            'displayType': displayTypeH
        },
        'data': contentsH
    }
    ]


def processElement(sourceZip, targetZip, elementType, elementPath):
    if (elementType == 'c'):
        return processCollection(sourceZip, targetZip, elementPath)
    elif (elementType == 'p'):
        return processDisplay(sourceZip, targetZip, elementPath)
    elif (elementType == 'm'):
        return processMC(sourceZip, targetZip, elementPath)
    elif (elementType == 'i'):
        return processQuestion(sourceZip, targetZip, elementPath)
    else:
        raise ImportError("Unknown element type")


def processAssignment(sourceZip, targetZip, assignmentPath):
    source = loadXML(sourceZip, assignmentPath)

    children = processChildren(sourceZip, targetZip, source['contents'])
    childrenPaths = [child['path'] for child in children if 'indirect' not in child]

    return {
        'path': source['title'],
        'meta': {
            'type': 'Assignment',
            'children': childrenPaths,
            'teamwork': False
        },
        'children': children
    }


def processLab(sourceZip, targetZip, labPath):
    source = loadXML(sourceZip, labPath)

    preLabPath = int(source['prelabCollectionIdx'])
    labPath = int(source['labCollectionIdx'])

    preLabPath = findElementById(sourceZip, 'c', preLabPath)
    labPath = findElementById(sourceZip, 'c', labPath)

    preLab = processAssignment(sourceZip, targetZip, preLabPath)
    lab = processAssignment(sourceZip, targetZip, labPath)

    preLab['meta']['teamwork'] = False
    lab['meta']['teamwork'] = True

    children = [preLab['path'], lab['path']]

    def processChildren(currentPath, element):
        element['path'] = os.path.join(currentPath, element['path'])

        saveElement(targetZip, element)

        if 'children' in element:
            for child in element['children']:
                processChildren(element['path'], child)

    processChildren('content', lab)
    processChildren('content', preLab)

    course = {
        'path': 'course',
        'meta': {
            'type': 'Course',
            'children': children
        }
    }

    saveElement(targetZip, course)


def copyImages(sourceZip, targetZip):
    regex = re.compile('.*/images/(.*)')

    for filePath in sourceZip.namelist():
        match = regex.search(filePath)

        if match is not None:
            targetPath = 'images/' + match.group(1)
            data = sourceZip.read(filePath)
            targetZip.writestr(targetPath, data)


sourceZipPath = sys.argv[1]
sourceZip = ZipFile(sourceZipPath, 'r')

targetZipPath = sys.argv[2]
targetZip = ZipFile(targetZipPath, 'w')

labPathRegex = re.compile(".*/data/l[0-9]{7}.txt")
labPath = [x for x in sourceZip.namelist() if labPathRegex.match(x)][0]

processLab(sourceZip, targetZip, labPath)
copyImages(sourceZip, targetZip)
