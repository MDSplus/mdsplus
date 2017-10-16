import smtplib
import requests
import json
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from datetime import datetime
from dateutil import tz

def skipIt(issue):
    for label in issue['labels']:
        if label['name'] == 'noremind':
          return True
    return False

def issueText(issue):
    issue['emailCreated']=datetime.strptime(issue['created_at'],"%Y-%m-%dT%H:%M:%SZ").replace(tzinfo=tz.tzutc()).astimezone(tz.tzlocal()).strftime("%m/%d/%Y")
    issue['emailLabels']=''
    for label in issue['labels']:
        issue['emailLabels']+=label['name']+" "
    assignees=''
    for assignee in issue['assignees']:
        assignees+=assignee['login']+" "
    if len(assignees) == 0:
        issue['emailColspan']=' colspan="2"'
        issue['emailAssignees']=''
    else:
        issue['emailColspan']=''
        issue['emailAssignees']='<td>%s</td>'%assignees
    text="""\
      <tr>
        <td>%(emailCreated)s</td>
        <td><a href="https://github.com/MDSplus/mdsplus/issues/%(number)d">%(title)s</a></td>
        <td%(emailColspan)s>%(emailLabels)s</td>
        %(emailAssignees)s
      </tr>
"""%issue
    return text

def assignedTo(issue):
    ans=list()
    for pers in issue['assignees']:
        ans.append(pers['login'])
    return ans

def messageContent(issues,user):
    total=0
    content="""\
<html>
  <head>
    <style>
    table, th, td {
      border: 1px solid black;
    }
    </style>
  </head>
  <body>
    <h2>You have outstanding MDSplus issues to deal with.</h2><br>
    <table>
      <tr>
        <th colspan="4">Issues assigned to you</th>
      </tr>
      <tr>
        <th>Created</th>
        <th>Title</th>
        <th>Labels</th>
        <th>Assignees</th>
      </tr>
"""
    num=0
    for issue in issues:
        if 'pull_request' not in issue:
            assigned=assignedTo(issue)
            if user in assigned and not skipIt(issue):
                num+=1
                content += issueText(issue)
    total += num
    content += """\
      <tr>
        <th colspan="4">Total assigned issues: %d</th>
      </tr>
      <tr>
        <th colspan="4">Issues still currently unassigned</th>
      </tr>
"""%num
    num=0
    for issue in issues:
        if 'pull_request' not in issue:
            assigned=assignedTo(issue)
            if len(assigned) == 0 and not skipIt(issue):
                num+=1
                content += issueText(issue)
    total += num
    content+= """\
      <tr>
        <th colspan="4">Total unassigned issues: %d</th>
      </tr>
    </table>
  </body>
</html>
"""%num
    if total > 0:
      return content
    else:
      return none
  

def getIssuesFromGithub():
    issues=list()
    page=1
    while True:
        r = requests.get("https://api.github.com/repos/MDSplus/mdsplus/issues",params={'state':'open','page':page})
        if r.ok:
            page+=1
            new_issues=json.loads(r.text)
            if len(new_issues) > 0:
                f=open('issues-%d.txt'%page,'w')
                f.write(r.text.encode('utf-8'))
                f.close()
                issues+=new_issues
            else:
                break
    return issues

def getIssuesFromFiles():
    issues=list()
    page=1
    while True:
        page+=1
        try:
            f=open('issues-%d.txt'%page,'r')
            issues=issues+json.load(f)
        except:
            break
    return issues

def sendIssueNotifications(users):
  issues=getIssuesFromGithub()
  for user in users.keys():
      msgtext=messageContent(issues,user)
      if msgtext is None:
          continue
      msg = MIMEText(msgtext,'html')
      msg['Subject']="MDSplus issues"
      msg['From']="twf@psfc.mit.edu"
      msg['To']=users[user]
      s=smtplib.SMTP('mail1.psfc.mit.edu')
      s.sendmail("twf@psfc.mit.edu",users[user],msg.as_string())
      s.quit()

