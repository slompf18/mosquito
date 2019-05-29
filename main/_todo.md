Todos for release

I (341599) wifi: state: run -> init (6c0)
I (341599) wifi: pm stop, total sleep time: 304560941 us / 337928134 us

I (341599) wifi: new:<10,0>, old:<10,0>, ap:<255,255>, sta:<10,0>, prof:1
E (341609) homekit: hk_com.c(76)-> 60 - Error receiving data: -1
E (341609) homekit: hk_com.c(76)-> 58 - Error receiving data: -1
E (341619) homekit: hk_com.c(76)-> 57 - Error receiving data: -1
D (341629) homekit: hk_com.c(228)-> 60 - Closing connection.
D (341639) homekit: hk_com.c(228)-> 58 - Closing connection.
D (341639) homekit: hk_com.c(228)-> 57 - Closing connection.
I (341729) wifi: new:<10,0>, old:<10,0>, ap:<255,255>, sta:<10,0>, prof:1
I (341729) wifi: state: init -> auth (b0)
I (341729) wifi: state: auth -> assoc (0)
I (341739) wifi: state: assoc -> run (10)
I (341739) wifi: connected with Ways, channel 10, bssid = 78:29:ed:40:80:c3
I (341789) wifi: pm start, type: 1

- check if full type is needed

- set mac adress
- check free heap over runtime
- write test plan
- Test recover after router reboot
- Test recovering after wrongly entered pin

- Document all header files
- Document project start
- create exampe projects
- Move to gitlab


Todos for completeness
- Implement list paring
- Implement add pairing over url


Ideas
- introduce hk_string beside hk_buffer
- debug
- implement bluetooth