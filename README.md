Dynamic Conditional Display in HTML
===================================

JavaScript for runtime dynamic display of conditional elements from DITA and other sources.

We use this in DITA2Go (http://dita2go.com) and Mif2Go (http://mif2go.com).  Put the runtime JS library, runcond.js, in the same dir as the HTML.  The HTML file itself contains, in its head, a reference to the JS file and a job-specifc "var CondAttrs".  The body element gets onload="SetDocConditions()", which sets the initial show/hide state of the conditional elements.  Those elements each have classes specifying their conditions, like class="audience_admin audience_expert".

The user can then show/hide conditions using a toolbar inserted wherever desired in the HTML.  In the test case conddita, for which source and HTML zips are provided, the toolbar is inserted in a table row, but it might also be at top and bottom of the page.  The settings in the "dynamic filtering" section of _d2html.ini (in condditasrc.zip) provide the info needed by DITA2Go; for other processors, you'll want something equivalent.  It's pretty obvious how it is used if you look at the HTML in condditahtml.zip.

If you want to see the variables and functions used to support this functionality in the DITA2Go C++ modules, study codesnippets.cpp.

Feel free to contact us if you want further details.
