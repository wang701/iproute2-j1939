#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <linux/can/j1939.h>

#include "utils.h"

/*
 * fill an link_af message from program arguments
 */
int j1939_link_args(int argc, char *argv[], struct nlmsghdr *msg, int msg_size)
{
	int saved_argc = argc;
	struct rtattr *afspec, *can, *j1939;
	uint8_t enable;

	NEXT_ARG();
	if (strcmp(*argv, "on") == 0) {
		enable = 1;
	} else if (strcmp(*argv, "off") == 0) {
		enable = 0;
	} else {
		enable = 1;
		/* revert arguments */
		++argc;
		--argv;
	}

	afspec = addattr_nest(msg, msg_size, IFLA_AF_SPEC);
	can = addattr_nest(msg, msg_size, AF_CAN);
	j1939 = addattr_nest(msg, msg_size, CAN_J1939);
	addattr_l(msg, msg_size, IFLA_J1939_ENABLE, &enable, sizeof(enable));
	addattr_nest_end(msg, j1939);
	addattr_nest_end(msg, can);
	addattr_nest_end(msg, afspec);

	return saved_argc - argc;
}

/*
 * process the returned IFLA_AF_SPEC/AF_CAN/CAN_J1939 attribute
 */
const char *j1939_link_attrtop(struct rtattr *nla)
{
	static char str[32];
	int pos;
	struct rtattr *tb[IFLA_J1939_MAX];

	pos = 0;
	str[0] = 0;
	parse_rtattr_nested(tb, IFLA_J1939_MAX-1, nla);
	if (tb[IFLA_J1939_ENABLE]) {
		uint8_t *u8ptr;

		u8ptr = RTA_DATA(tb[IFLA_J1939_ENABLE]);
		pos += sprintf(&str[pos], "j1939 %s", *u8ptr ? "on" : "off");
	}
	return str;
}
