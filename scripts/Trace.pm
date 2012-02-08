package Trace;

#--- LOGGING
use constant {
    LOG_DEBUG => 1,
    LOG_INFO  => 2,
    LOG_MESSAGE => 3,
};
our @EXPORT = qw(debug info message $LOG_LEVEL LOG_DEBUG LOG_INFO LOG_MESSAGE);
#our @EXPORT_OK = qw($LOG_LEVEL LOG_DEBUG LOG_INFO LOG_MESSAGE);
#-----

use strict;
use Time::Local;
use Exporter;
our @ISA = qw(Exporter);

our $LOG_LEVEL = LOG_DEBUG;

sub debug {
    if ($LOG_LEVEL <= LOG_DEBUG) {
	&trace("DEBUG", $_[0]);
    }
}

#
#--------------------------------------------------------------------------------------
#

sub info {
    if ($LOG_LEVEL <= LOG_INFO) {
	&trace("INFO", $_[0]);
    }
}

#
#--------------------------------------------------------------------------------------
#

sub message {
    if ($LOG_LEVEL <= LOG_MESSAGE) {
	&trace("MSG", $_[0]);
    }
}

#
#--------------------------------------------------------------------------------------
#


sub trace {
    my($lvl, $str);
    ($lvl, $str) = @_;

    my $timestamp = localtime();
    print "[$lvl]\t$timestamp: $str\n";
}

1;
