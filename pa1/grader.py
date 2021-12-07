#!/usr/bin/env python3
import autograde
import os, os.path

assignment_name = 'PA1'
release='1'

autograde.Test.time_limit = 60

class ErrorStringTests(autograde.StringTests):
    @staticmethod
    def Test(**kws):
        return autograde.RefTest(ref_code = 1, **kws)

assignment = autograde.MultiProject(
    autograde.StringTests.Project('yell'),
    autograde.StringTests.Project('rle', weight=0.625),
    autograde.Project('balance',
        autograde.StringTests(id = '1', name = '', weight = 0.5),
        ErrorStringTests(id = '2', name = '', weight = 0.5)
    ),
    autograde.StdinFileTests.Project('list', weight=2),
    autograde.FileTests.Project('mexp', weight=2),
    autograde.Project('bst',
        autograde.StdinFileTests(id='1', weight=1.5),
        autograde.StdinFileTests(id='2', weight=1.5),
    )
)


if __name__ == '__main__':
    autograde.main(assignment_name, assignment, release)
