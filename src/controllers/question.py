import random
import yaml

from flask import g

from entities import MultipleChoiceQuestionElement, AnswerContent


class MultipleChoiceQuestionController:
    def __init__(self, element):
        self.element = element

        self.answer = self.element.getAnswer(g.user)
        self.needTeamAnswer = self.element.needTeamAnswer()
        self.secret = self.element.getSecret()

        if self.needTeamAnswer:
            self.userAnswer = self.element.getUserAnswer(g.user)
        else:
            self.userAnswer = self.answer

        self.hasCorrection = self.answer.hasCorrection()
        self.hasContent = self.answer.hasContent()

        self.setupShuffle()
        self.order = yaml.load(self.userAnswer.meta)

    @classmethod
    def fromParams(self, course, branch, path, meta=None):
        return MultipleChoiceQuestionController(MultipleChoiceQuestionElement(course, branch, path, meta))

    def renderParams(self):
        return {
            'element': self.element,
            'controller': self,
            'answer': self.answer,
            'userAnswer': self.userAnswer,
            'hasCorrection': self.hasCorrection,
            'hasContent': self.hasContent,
            'shuffle': self.do_shuffle(range(self.element.meta['optionCount'])),
            'secret': self.secret,
            'needTeamAnswer': self.needTeamAnswer
        }

    def setupShuffle(self):
        if self.userAnswer.meta is None:
            array = [i for i in range(0, self.element.meta['optionCount'])]

            if self.element.meta['shuffle']:
                random.shuffle(array)

            self.userAnswer.meta = yaml.dump(array)
            self.userAnswer.save()

    def isCorrect(self, answer):
        return self.incorrectCount(answer) <= self.element.meta['maxAllowedMistakes']

    def incorrectCount(self, answer):
        incorrectCount = 0

        for idx, val in enumerate(self.secret['options']):
            if val != answer[self.order[idx]]:
                incorrectCount = incorrectCount + 1

        return incorrectCount

    def correctArray(self, answer):
        return [val != answer[self.order[idx]] for idx, val in enumerate(self.secret['options'])]

    def isLatestCorrect(self):
        if not self.hasCorrection:
            return False
        return yaml.load(self.answer.getLatestCorrection().correction)['isCorrect']

    def canAnswer(self):
        return not self.isLatestCorrect() and self.answer.contents.count() < self.element.meta['maxAllowedAnswers']

    def processAnswer(self, answers):
        if not self.canAnswer():
            return

        answers = self.do_unshuffle(answers)

        answercontent = AnswerContent(answer=self.userAnswer, content=yaml.dump(answers))
        answercontent.save()

        if self.needTeamAnswer:
            team = g.user.getTeamForCourse(self.element.course)

            if self.element.getTeamAnswer(g.user).hasContent():
                lastTeamAnswerTime = self.element.getTeamAnswer(g.user).getLatestContent().time
            else:
                lastTeamAnswerTime = None

            for user in team.users:
                collegueAnswer = self.element.getUserAnswer(user)

                if not collegueAnswer.hasContent():
                    # Collegue has not yet answered
                    return

                data = collegueAnswer.getLatestContent()

                if lastTeamAnswerTime is not None and data.time < lastTeamAnswerTime:
                    # Collegue answer is older than last request
                    return

                collegueAnswers = yaml.load(data.content)

                if collegueAnswers != answers:
                    # No consensus
                    return

            # Everybody answered and there is a consensus

            teamanswercontent = AnswerContent(answer=self.answer, content=yaml.dump(answers))
            teamanswercontent.save()

        correction = {
            'isCorrect': self.isCorrect(answers),
            'incorrectCount': self.incorrectCount(answers),
            'correctArray': self.correctArray(answers)
        }

        if self.needTeamAnswer:
            teamanswercontent.correction = yaml.dump(correction)
            teamanswercontent.save()
        else:
            answercontent.correction = yaml.dump(correction)
            answercontent.save()

    def do_shuffle(self, data):
        return [data[idx] for idx in self.order]

    def do_unshuffle(self, data):
        revorder = [0 for x in range(len(self.order))]

        for idt, idx in enumerate(self.order):
            revorder[idx] = idt

        return [data[idx] for idx in revorder]

    def getPreviousAnswers(self):
        def processContent(answercontent):
            return self.do_shuffle(yaml.load(answercontent.content))

        return [processContent(content) for content in self.answer.contents]

    def getCollegueAnswers(self):
        if not self.needTeamAnswer:
            return []

        if self.element.getTeamAnswer(g.user).hasContent():
            lastTeamAnswerTime = self.element.getTeamAnswer(g.user).getLatestContent().time
        else:
            lastTeamAnswerTime = None

        def processUser(user):
            answer = self.element.getUserAnswer(user)
            if answer.hasContent():
                data = answer.getLatestContent()

                if lastTeamAnswerTime is not None and data.time < lastTeamAnswerTime:
                    # collegues answer coresponds to an older request
                    return None

                return self.do_shuffle(yaml.load(data.content))
            else:
                return None

        return [processUser(user) for user in self.getCollegues()]

    def getCollegues(self):
        return [user for user in g.user.getTeamForCourse(self.element.course).users]
