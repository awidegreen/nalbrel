#!/usr/bin/perl -w

use RefactorJob;
use Trace;
use strict;

$LOG_LEVEL = LOG_DEBUG;



my $numArgs = $#ARGV;
if ($numArgs < 1) {
    &print_help();
    exit 0;
} else {
    &main($ARGV[0], $ARGV[1]);
}

######################################################################################
######################################################################################
# subs #
######################################################################################

sub print_help {
    print "Usage: ./move_to_music.pl <source-dir> <target-dir>\n".
	  "  <source-dir>: should include all the albums which should be sorted in e.g. Artist - Album - Year\n".
	  "  <target-dir>: should include the folders 0-9, a, b, c, ... z. e.g. /music\n";
}

#
#--------------------------------------------------------------------------------------
#

sub main {
    my $source_dir = shift;
    my $target_dir = shift;

    my $refactor_job_hash = {};      # define job hash
    my $cnt = 1;
    my @files = <$source_dir/*>;
    foreach my $item (@files) {
	my $is_dir = -d $item;
	if ( $item =~ /^\./  ||
	     $item =~ /lost\+found/ || 
	     !$is_dir) {
	    next;
	}
	
	my $job = &analyse_item($item, $target_dir);
	if (defined($job)) {
	    $$refactor_job_hash{$cnt} = $job;
	    $cnt++;
	}
    }

    my $amount = keys(%$refactor_job_hash);
    if ($amount == 0) {
	&Trace::message("Nothing to do for me, gone for good!");
	return;
    }

    foreach my $job_nr ( keys(%$refactor_job_hash) ) {
	&Trace::message("Refactor Job nr: $job_nr");
	$$refactor_job_hash{$job_nr}->create_artist_dir();
	$$refactor_job_hash{$job_nr}->move_item();
    }
    &Trace::message("Congratulation, Everything is finished!!!\n");
}

#
#--------------------------------------------------------------------------------------
#

sub analyse_item {
    my $item = shift;
    my $target_dir = shift;
    &Trace::info("Going to analyse item: \"$item\"");

    $item =~ /(.*)\/(.*)/;   
    my $base_dir = $1;
    my $work_dir = $2;

    &Trace::debug("base dir: $base_dir   ;   work|album_dir: $work_dir");

    my $starts_with;
    if ($work_dir =~ m/^the ([a-z])/i) {  # Test for "The [a-z]"
	$starts_with = lc($1);
    } elsif ($work_dir =~ m/^the (\d)/i) { # Test for "The [0-9]"
	$starts_with = "0-9";
    } elsif ($work_dir =~ m/^([a-z])/i) { # Test for "[a-z]"
	$starts_with = lc($1);
    } elsif ($work_dir =~ m/^(\d)/i) { # Test for "[0-9]"
	$starts_with = "0-9";
    }

    &Trace::debug("Album starts with: ".$starts_with);

    $base_dir = $target_dir."/".$starts_with;
    unless (-d $base_dir) {
	die "The new target base dir does not exists: $base_dir";
    }
    &Trace::debug("target base dir: ".$base_dir);

    if ( $work_dir =~ /(.*?)\ -\ (.*)/ ) {
	my $artist = $1;
	my $album  = $2;
	my $year   = "";

	if ( $album =~ /(.*?)\ -\ (\d{4})/ ) {
	    $album = $1;
	    $year  = $2;
	}
	    
	&Trace::debug("artist: $artist   ;   album: $album");

	my $artist_dir = $base_dir."/".$artist;

	my $refactor_job = RefactorJob->new($item, $base_dir, $work_dir, $artist, $album, $year);
	return $refactor_job;
    } else {
	return undef;
    }
}
