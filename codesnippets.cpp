// C++ snippets from DITA2Go code


// runtime conditions in drxml.dll and drmxl.dll

bool XMLrd::FilterConditions = false;

// in drxml and drmxl, if attribute is in list,
// keep element and write class attr for it
DCnlist XMLrd::FilterDialogAttributes;
DCnlist *XMLrd::FilterClasses = NULL;
long XMLrd::FilterClassCount = 0;


void
XMLrd::SetFilterAttributes(void)
{
	// called during program initialization

	if (!FilterConditions)
		return;

	// read [FilterAttributes] and populate FilterDialogAttributes
	if (!IniFile
	 || !IniFile->Section("FilterAttributes"))
		return;
	while (IniFile->NextItem()) {
		char *aname = IniFile->StrLeft();
		char *vlist = IniFile->StrRight();
		DCnlist *npt = new DCnlist();
		npt->addlist(vlist);
		FilterDialogAttributes.add(aname, (long) npt);
		while ((npt = npt->next) != NULL) {
			npt->id *= 2;
			if (*(npt->name) == '-') {
				npt->name++;
				npt->id ++;
			}
		}
	}
}


bool
XMLrd::CheckFilterConds(const char **atts)
{
	// called from StartElement code
	bool condset = false;

	for (long i = 0; atts[i] != NULL; i += 2) {
		if (FilterConditions
		 && (FilterDialogAttributes.getitem(atts[i]) != 0)) {
			// reserve for dynamic filtering, generate classes
			const char *val = atts[i + 1];  // one or more values
			if (val) {
				condset = true;
				WriteFilterClass((char *) atts[i], (char *) val);
			}
		}
	}

	return condset;
}


void 
XMLrd::WriteFilterClass(char *aname, char *aval)
{
	if (!FilterConditions)
		return;

	// in drxml and drmxl, write class attribute value
	DCnlist *vpt = NULL;
	long val = 0;

	if ((vpt = (DCnlist *) FilterDialogAttributes.getitem(aname)) == NULL)
		return;  // attr name not in list

	if (!FilterClasses)
		FilterClasses = new DCnlist();

	char *spc = NULL;
	DCnlist *npt = new DCnlist();
	if (((spc = strchr(aval, ' ')) != NULL)
	 && (*(spc + 1) != '\0')) {  // is a list
		npt->addlist(aval);
	}
	else
		npt->add(aval, 1);

	char *cstr = NULL;
	char *astr = NULL;
	DCctl *cp = NULL;

	while ((npt = npt->next) != NULL) {
		if (((aval = (char *) npt->name) != NULL)) {
		// && ((val = vpt->getitem(aval)) != 0)) { // val is in list
			cstr = NewName(aname, strlen(aname) + strlen(aval) + 2);
			strcat(cstr, "_");
			strcat(cstr, aval);

			FilterClasses->add(cstr, ++FilterClassCount);
		}
	}
}


void XMLrd::SetFilterClasses(bool tbl, bool inl)
{
	// called at end of StartElement for tbl row, and 
	// after setting format for para and char elements

	if (!FilterClasses
	 || !TextGroup)
		return;

	if (tbl
	 && !TableRowGroup)
		return;

	DCnlist *dnp = FilterClasses;
	while ((dnp = dnp->next) != NULL) {
		const char *cstr = dnp->name;
		DCctl *cp = new DCctl(etext, 2, 110, 3);
		char *astr = NewName((tbl && TableRowGroup) ? "rowclass" :
			                   (inl ? "charclass" : "paraclass"), strlen(cstr) + 12);
		strcat(astr, ":");
		strcat(astr, cstr);
		strcat(astr, " ");
		cp->ex(astr);
		TextGroup->add(*cp);
	}
	delete FilterClasses;
	FilterClasses = NULL;
	FilterClassCount = 0;
}

// end of drxmldv.cpp


// runtime condition filtering in dwhtm.dll

bool HTMwr::FilterConditions = false;
char *HTMwr::FilterJSFile = "runcond.js";

char *HTMwr::FilterDialogTitle = "Select conditions to hide:";
char *HTMwr::FilterBodyAttr = "onclick=\"dialogWin.checkModal()\" onfocus=\"return dialogWin.checkModal()\"";
char *HTMwr::FilterLinkHref = "javascript:SetCondAttrs()";
uns HTMwr::FilterDialogWide = 250;
uns HTMwr::FilterDialogHigh = 300;

char *HTMwr::FilterSelectBar = NULL;
char *HTMwr::FilterBarStart = "Hide:";
char *HTMwr::FilterBarSpan = "<span class=\"show %s\" style=\"cursor:pointer;color:blue;\" onclick='ToggleShowHide(\"%s\")'>%s</span>";
char *HTMwr::FilterBarEnd = "or Show: <span class=\"show\" style=\"cursor:pointer;color:blue;\" onclick='ShowAll()'>All</span>";
char *HTMwr::FilterBarBodyAttr = "onload=\"SetDocConditions()\"";


// in drxml and drmxl, if attribute is in list,
// keep element and write class attr for it
// in dwhtm, just write as CondAttrs
DCnlist HTMwr::FilterDialogAttributes;


void
HTMwr::SetFilterAttributes(void)
{
	if (!FilterConditions)
		return;

	// read [FilterAttributes] and populate FilterDialogAttributes
	if (!IniFile
	 || !IniFile->Section("FilterAttributes"))
		return;
	while (IniFile->NextItem()) {
		char *aname = IniFile->StrLeft();
		char *vlist = IniFile->StrRight();
		DCnlist *npt = new DCnlist();
		npt->addlist(vlist);
		FilterDialogAttributes.add(aname, (long) npt);
		while ((npt = npt->next) != NULL) {
			npt->id *= 2;
			if (*(npt->name) == '-') {
				npt->name++;
				npt->id ++;
			}
		}
	}
}


void 
HTMwr::WriteFilterDialogSettings(DCwfile *wf)
{
	if (!FilterConditions)
		return;

	DCnlist *npt = &FilterDialogAttributes;
	DCnlist *vpt = NULL;

	// write var CondAttrs to top of HTML files, or to OH file *_ohr.js
	wf->putStr("var CondAttrs = [\n");
	while ((npt = npt->next) != NULL) {
		wf->putStr("\t[\"");
		wf->putStr(npt->name);
		wf->putStr("\", [ ");
		vpt = (DCnlist *) npt->id;
		while ((vpt = vpt->next) != NULL) {
			wf->putStr("[\"");
			wf->putStr(vpt->name);
			wf->putStr("\", ");
			wf->putChar((vpt->id & 1) + '0');
			wf->putStr("]");
			if (vpt->next)
				wf->putStr(", ");
			else
				wf->putStr(" ]");
		}
		wf->putStr(" ]");
		if (npt->next)
			wf->putStr(",\n");
		else
			wf->putStr(" ];\n");
	}
}


char * 
HTMwr::WriteFilterSelectBar(void)
{
	if (!FilterConditions)
		return NULL;

	// invoke from predefined macro var <$$_FilterBar>
	// write out the contents of the selection bar
	char bar[FilterSelectBarMax];
	char *cpt = bar;
	DCnlist *attr = &FilterDialogAttributes;
	const char *aname = NULL;
	const char *aval = NULL;
	DCnlist *vlist = NULL;
	char *cstr = NULL;
	char *tstr = NULL;
	bool useini = false;

	// write start from [FilterConditions]FilterBarStart
	if (FilterBarStart) {
		strcpy(cpt, FilterBarStart);
		strcat(cpt, " \n");
	}

	// for each name/value:
	//   write span per format str in [FilterConditions]
	//   span text is in [FilterValueText] indexed by attr_value
	if (IniFile
	 && IniFile->Section("FilterValueText"))
		useini = true;

	while ((attr = attr->next) != NULL) {
		aname = attr->name;
		vlist = (DCnlist *) attr->id;
		while ((vlist = vlist->next) != NULL) {
			aval = vlist->name;
			cstr = NewName(aname, strlen(aname) + strlen(aval) + 2);
			strcat(cstr, "_");
			strcat(cstr, aval);
			if (useini && IniFile->Find(cstr))
				tstr = IniFile->StrRight();
			else
				tstr = cstr;
			cpt += strlen(cpt);
			sprintf(cpt, FilterBarSpan, cstr, cstr, tstr ? tstr : cstr);
			if (tstr != cstr)
				DeleteName(tstr);
			DeleteName(cstr);
			tstr = cstr = NULL;
			strcat(cpt, "\n");
		}
	}

	// write end from [FilterConditions]FilterBarEnd
	if (FilterBarEnd) {
		strcat(cpt, " ");
		strcat(cpt, FilterBarEnd);
	}

	return NewName(bar);
}


void 
HTMwr::WriteFilterDisplayStyle(HTMwfile *wf)
{
	// write style attribute value "display: none;"
	if (!FilterConditions)
		return;

	wf->strAttr("style", "display: none;");
}


// end of C++ snippets from DITA2Go

