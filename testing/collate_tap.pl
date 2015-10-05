use strict;
use POSIX qw/strftime/;
use TAP::Parser qw/all/;
use TAP::Parser::Aggregator qw/all/;
use TAP::Stream;
use TAP::Stream::Text;


my $stream = TAP::Stream->new;

open my $out_file, ">>", "./test_report.txt"
    or die "Cannot open outfile. $!\n";

open my $out_tap, ">>", "./tapout.tap"
    or die "cacca $!\n";

printf $out_file "\n------------------------------------------------------------------\n";
printf $out_file "Report %s",
                  strftime("%y%m%d-%H:%M", localtime);
printf $out_file "\n------------------------------------------------------------------\n";


my @files = qw[
               ./example1_out.tap
               ./example2_out.tap
              ];

my $aggregate = TAP::Parser::Aggregator->new;

my $test_count = 1;

foreach my $file (@files) {
    my $parser = TAP::Parser->new( { source => $file } );
    while ( my $result = $parser->next ) {
        ## printf $out_file "$file results: %s\n", $result->as_string;
        if( $result->is_test ) {
        # printf $out_file "%s %d %s \n", $result->ok, $test_count, $result->description;
        # printf $out_tap  "%s %d %s \n", $result->ok, $test_count, $result->description;
        # printf $out_file "%s\n\n", $result->as_string;
        $test_count += 1;
        }
    }
        
    my $document = do {
       local $/ = undef;
       open my $fh, "<", $file
          or die "could not open $file: $!";
       <$fh>;
    };

    # printf $out_file $document;
    

    $stream->add_to_stream( TAP::Stream::Text->new(name => $file, text => $document) );

    ## printf $out_file "\nPlanned: %s\nPassed: %s\nFailed: %s\n",
    ## $parser->tests_planned, scalar $parser->passed, scalar $parser->failed;

    $aggregate->add($file, $parser);
}





printf $out_file $stream->to_string;

# printf $out_tap "1..%d", $aggregate->total;


# printf $out_file "\nTotal: %s\nPlanned: %s\nPassed: %s\nFailed: %s\n",
#    $aggregate->total, scalar $aggregate->planned, scalar $aggregate->passed, scalar $aggregate->failed;

close $out_file;
close $out_tap;

