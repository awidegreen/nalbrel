#!/usr/bin/perl -w

use RefactorJob;
use Trace;
use strict;

$LOG_LEVEL = LOG_DEBUG;


my $numArgs = $#ARGV;
if ($numArgs < 0) {
    &printHelp;
} else {
    &main($ARGV[0]);
}

########################################################################################


sub main {
    my $main_dir = shift;       # read first program argument
    my @files = <$main_dir/*>;  # read direcotry

    my $refactor_job_hash = {};      # define job hash
    my $cnt = 1;
    foreach my $item (@files) {
	my $is_dir = -d $item;
	if ( $item =~ /^\./  ||
	     $item =~ /lost\+found/ || 
	     !$is_dir) {
	    next;
	}

	#&refactor_subdir($item);
	my $job = &analyse_item($item);
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
    
    &Trace::message("Found ".@files." sub-dirs in given directory, with ".$amount." potential candidates\n");

    &print_job_hash($refactor_job_hash);

    print "Do you want to remove items from the list?\n".
  	  "  \"0\" no thanks, do it as printed\n".
	  "  \"1,3,4,5,32,9,..,43\" for the items that should be removed (comma seperated)!\n";
	  
    my $line_input;
    my $in_ok = 0;
    while ($in_ok == 0) {
      print "items> ";
      while (defined(chomp($line_input = <STDIN>))) {
  	  if ($line_input =~ m/^\s*0\s*$/) {
	      print "Ok, nothing is deleted\n";
	      $in_ok = 1;
	      last;
	  }
	  if ($line_input =~ m/(^\s*([\d|,]*)\s*)/) {
	      print "The following items has been removed: ";
	      foreach (split /,/, $1) {
		  print "$_ ";
		  delete $$refactor_job_hash{$_};
	      }
	      print "\n";
	      	      
	      $in_ok = 1;
	      last;
	  }
	  print "Could not recognize this one, again!\n";
	  last;
      }
    }

    #&print_job_hash($refactor_job_hash);
    $amount = keys(%$refactor_job_hash);
    foreach my $job_nr ( sort numeric_sort keys(%$refactor_job_hash) ) {
	&Trace::message("Refactor Job nr: $job_nr/$amount");
	$$refactor_job_hash{$job_nr}->create_artist_dir();
	$$refactor_job_hash{$job_nr}->move_item();
    }
    &Trace::message("Congratulation, Everything is finished!!!\n");
    
}	


#
#--------------------------------------------------------------------------------------
#

sub print_job_hash {
    my $job_hash = shift;

    foreach my $job_nr ( sort numeric_sort keys(%$job_hash) ) {
	printf "[%3d] \"".$$job_hash{$job_nr}->get("_artist")."\" for \"".$$job_hash{$job_nr}->get("_item")."\"\n", $job_nr;
    }
}

#
#--------------------------------------------------------------------------------------
#

sub numeric_sort { $a <=> $b }

#
#--------------------------------------------------------------------------------------
#



sub analyse_item {
    my $item = shift;
    &Trace::info("Going to analyse item: \"$item\"");

    $item =~ /(.*)\/(.*)/;   
    my $base_dir = $1;
    my $work_dir = $2;

    &Trace::debug("base dir: $base_dir   ;   rest: $work_dir");
    
    if ( $work_dir =~ /(.*?)\ -\ (.*)/) {
	my $artist = $1;
	my $album  = $2;
	&Trace::debug("artist: $artist   ;   album: $album");

	my $year = "";
	if ( $album =~ /.*?\ -\ (.\d{4})/) {
	    $year = $1;
	}

	my $artist_dir = $base_dir."/".$artist;

	my $refactor_job = RefactorJob->new($item, $base_dir, $work_dir, $artist, $album, $year);
	return $refactor_job;
    } else {
	return undef;
    }
}


#
#--------------------------------------------------------------------------------------
#

#sub refactor_subdir {
#    (my $item) = @_;
#    &Trace::info("Going to refactor item: $item");
#
#   $item =~ /(.*)\/(.*)/;   
#    my $base_dir = $1;
#    my $work_dir = $2;
#
#    &Trace::debug("base dir: $base_dir   ;   rest: $work_dir");
    
#    if ( $item =~ /([\w|\ |-]*?)\ -\ ([\w|\ |-]*)/) {
#    if ( $work_dir =~ /(.*?)\ -\ (.*)/) {
#	my $artist = $1;
#	my $album  = $2;
#	&Trace::debug("artist: $artist   ;   album: $album");
#
#	my $artist_dir = $base_dir."/".$artist;
#	print "Artist: \"$artist\", do you want to create a sperate directory? (y/n) \nActions which are performed:\n";
#	print "\tCreate artist directory:\t\"$artist_dir\"\n";
#	print "\tMove existing directory:\t\"$item\" to \n\t\t\t\t\t\"$artist_dir\/$work_dir\"\n";
#	print "> ";
#
#	my $perform_action = 0;
#	while (defined(chomp(my $line = <STDIN>))) {
#	    if ($line =~ /^(y)|(n)/i) {
#		#&Trace::debug("1: $1 - 2: $2\n");
#		if (defined($1)) {
#		    $perform_action = 1;
#		    last;
#		} elsif (defined($2)) {
#		    $perform_action = 0;
#		    last;
#		}
#		else {
#		    #print "Error: could not recognize your input, again (y/n)\n> ";
#		    redo;
#		}		
#	    }
#	}
#
#	if ($perform_action == 1) {
#	    mkdir  $artist_dir;
#	    rename $item , "$artist_dir\/$work_dir";
#	    print "Finished with Artist: \"$artist\"";
#	   #print "DO CREATING AND MOVEMENT!\n";
#	} else {
#	    print "DO NOTHING\n";
#	}
#	print "\n\n";
#    }
#}


#
#--------------------------------------------------------------------------------------
#


sub printHelp {
    print "This script requires at least one parameter, the folder to be refactored!\n".
          "\tusage: refactormusic.pl /path/to/music\n";
}

#
#--------------------------------------------------------------------------------------
#





