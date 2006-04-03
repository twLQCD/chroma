#
#  $Id: regres.pl,v 3.0 2006-04-03 04:59:28 edwards Exp $
#
#  This is the portion of a script this is included recursively
#

#
# Each test has a name, input file name, output file name,
# and the good output that is tested against.
#
@regres_list = 
    (
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/spectrumOct/spectrumOct-v11.ini.xml" , 
	 output      => "spectrumOct-v11.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/spectrumOct/spectrumOct-v11.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/spectrumOct/spectrumOct-v11.out.xml" ,
     }
     );
