/*
 * Copyright (c) 2000 Silicon Graphics, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 * Mountain View, CA  94043, or:
 *
 * http://www.sgi.com
 *
 * For further information regarding this notice, see:
 *
 * http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */
/* $Id$ */
/**********************************************************
 * 
 *    OS Test - Silicon Graphics, Inc.
 * 
 *    TEST IDENTIFIER	: dup02
 * 
 *    EXECUTED BY	: anyone
 * 
 *    TEST TITLE	: Negative test for dup(2) with bad fd.
 * 
 *    PARENT DOCUMENT	: usctpl01
 * 
 *    TEST CASE TOTAL	: 2
 * 
 *    WALL CLOCK TIME	: 1
 * 
 *    CPU TYPES		: ALL
 * 
 *    AUTHOR		: Richard Logan
 * 
 *    CO-PILOT		: William Roske
 * 
 *    DATE STARTED	: 06/94
 * 
 *    INITIAL RELEASE	: UNICOS 7.0
 * 
 *    TEST CASES
 * 
 * 	1-?.) dup(2) returns -1 with errno set to EBADF...(See Description)
 *	
 *    INPUT SPECIFICATIONS
 * 	The standard options for system call tests are accepted.
 *	(See the parse_opts(3) man page).
 * 
 *    OUTPUT SPECIFICATIONS
 *	Standard tst_res formatted output
 * 	
 *    DURATION
 * 	Terminates - with frequency and infinite modes.
 * 
 *    SIGNALS
 * 	Uses SIGUSR1 to pause before test if option set.
 * 	(See the parse_opts(3) man page).
 *
 *    RESOURCES
 * 	None
 * 
 *    ENVIRONMENTAL NEEDS
 *      No run-time environmental needs.
 * 
 *    SPECIAL PROCEDURAL REQUIREMENTS
 * 	None
 * 
 *    INTERCASE DEPENDENCIES
 * 	None
 * 
 *    DETAILED DESCRIPTION
 * 
 * 	Setup:
 * 	  Setup signal handling.
 *	  Pause for SIGUSR1 if option specified.
 * 
 * 	Test:
 *	 Loop if the proper options are given.
 *	  Loop through the test cases
 * 	   Execute system call
 *	   Check return code, if system call failed (return=-1)
 *		if doing functional check
 *		   check if errno set correctly, report results
 *	  Otherwise, Issue a FAIL message.
 * 
 * 	Cleanup:
 * 	  Print errno log and/or timing stats if options given
 * 
 * 
 *#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#**/

#include <sys/types.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "test.h"
#include "usctest.h"

extern void setup();
extern void cleanup();


char *TCID="dup02"; 		/* Test program identifier.    */
int TST_TOTAL=2;    		/* Total number of test cases. */
extern int Tst_count;		/* Test Case counter for tst_* routines */

int exp_enos[]={0, 0};

int Fds[] = { -1, 1500 };


/***********************************************************************
 * Main
 ***********************************************************************/
int
main(int ac, char **av)
{
    int lc;		/* loop counter */
    const char *msg;		/* message returned from parse_opts */
    int nfds = sizeof(Fds) / sizeof(int);
    int ind;
    
    /***************************************************************
     * parse standard options
     ***************************************************************/
    if ( (msg=parse_opts(ac, av, (option_t *)NULL, NULL)) != (char *) NULL ) {
	tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);
	tst_exit();
    }

    /***************************************************************
     * perform global setup for test
     ***************************************************************/
    setup();

    /* set the expected errnos... */
    TEST_EXP_ENOS(exp_enos);

    /***************************************************************
     * check looping state if -c option given
     ***************************************************************/
    for (lc=0; TEST_LOOPING(lc); lc++) {

	/* reset Tst_count in case we are looping. */
	Tst_count=0;

	for (ind=0; ind<nfds; ind++) {

	    /* 
	     * Call dup(2)
	     */
	    TEST( dup(Fds[ind]) );
	
	    /* check return code */
	    if ( TEST_RETURN == -1 ) {
	        if ( STD_FUNCTIONAL_TEST ) {
		    if ( TEST_ERRNO == EBADF ) {
	                tst_resm(TPASS, "dup(%d) Failed, errno=%d : %s", Fds[ind],
		            TEST_ERRNO, strerror(TEST_ERRNO));
		    }
		    else  {
	                tst_resm(TFAIL,
			    "dup(%d) Failed, errno=%d %s, expected %d (EBADF)",
			    Fds[ind], TEST_ERRNO, strerror(TEST_ERRNO), EBADF);
		    }
	        }
	    } else {
	        tst_resm(TFAIL, "dup(%d) returned %d, expected -1, errno:%d (EBADF)",
		    Fds[ind], TEST_RETURN, EBADF);

	        /* close the new file so loops do not open too many files */
                if (close(TEST_RETURN) == -1) {
                   tst_brkm(TBROK, cleanup, "close(%d) Failed, errno=%d : %s",
		        TEST_RETURN, errno, strerror(errno));
                }
	    }
	}
    }	/* End for TEST_LOOPING */

    /***************************************************************
     * cleanup and exit
     ***************************************************************/
    cleanup();

    return 0;
}	/* End main */

/***************************************************************
 * setup() - performs all ONE TIME setup for this test.
 ***************************************************************/
void 
setup()
{
    /* capture signals */
    tst_sig(FORK, DEF_HANDLER, cleanup);

    /* make a temp directory and cd to it */
    tst_tmpdir();

    /* Pause if that option was specified */
    TEST_PAUSE;

}	/* End setup() */


/***************************************************************
 * cleanup() - performs all ONE TIME cleanup for this test at
 *		completion or premature exit.
 ***************************************************************/
void 
cleanup()
{
    /*
     * print timing stats if that option was specified.
     * print errno log if that option was specified.
     */
    TEST_CLEANUP;

    /* Remove tmp dir and all files in it */
    tst_rmdir();

    /* exit with return code appropriate for results */
    tst_exit();
}	/* End cleanup() */


