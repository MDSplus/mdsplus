\anchor javascope_manual

>
> This is the user manual of a specific MDSplus software component, For a
> general description of the overall MDSplus framework please refer to:
>
>  * \ref mdsplus_manual "The Reference manual"
>  * [The MDSplus site](http://www.mdsplus.org/index.php)
>

\htmlonly 
<blockquote class="doxtable">
<p>A pdf version of this manual is available here: <a href="../javascope.pdf">The jScope manual</a></p>
</blockquote> 
\endhtmlonly



The MDSplus java scope
======================

The jScope application is one of the most used tool for data visualization of
MDSplus equipped fusion experiments. It is widely adopted in the fusion
community for it simplicity, versatility and portability. The main goal of the
application is to provide the user with a direct visualization of experimental
data plotting signals as visual envelopes or showing image frames. It is simple
in the way that it was primarily intended to be a graphical user interface and
none of the data structure internals is exposed to the user. It is versatile
because it can manage in principle any kind of formatted data with the same
interface to the user. Finally it is highly portable as it is a self-consisted
multi-platform application. 


\image html img/jscope_view.jpg "jScope view example showing an experiment shot"
\image latex img/jscope_view.jpg "jScope view example showing an experiment shot" height=6cm


jScope was designed as a direct evolution of the MDSplus dwscope and it lives
inside the MDSplus software package although in principle it would be able to
display data in different formats trough the definition of a specific reader
object that is called the “Data Provider” (see \ref jscope_dataprovider).
In any case the way the data are presented fits the form of the MDSplus naming
(i.e. the tree path data hierarchy naming and shots temporal subdivision).

In a network connection the MdsDataProvider is always interactively talking
with the server asking for a proper resampling of the real data to boundaries
and resolution suggested by the actual plot that has to be shown by scope in
that moment. This can dramatically reduce the download time required to render
the active plots.
