#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

private bool only_integer_ = true;

void tnumbox::insert_char(const Uint16 unicode)
{	
	// this isn't full localization support, but it may help
	// perhaps one could check against something like
	// char ds = loc.decimal_point()
	if (unicode == ',') {
		unicode = '.';
	}
	
	// if the char is no digit and no dot, skip it
	// if only_integer=true or there is already another dot, skip it
	if (!isdigit(unicode) && (only_integer_ || unicode !=  '.' || text_.text().find('.') != std::string::npos)) {
		return;
	}
	
	delete_selection();
	if(text_.insert_unicode(selection_start_, unicode)) {

		// Update status
		set_cursor(selection_start_ + 1, false);
		update_canvas();
		set_is_dirty(true);
	}
}




void tnumbox::paste_selection(const bool mouse)
{
	const std::string& text = copy_from_clipboard(mouse);
	
	// trim leading and trailing whitespaces
	boost::algorithm::trim(text);
	// is something left?
	if(text.empty()) return;
	// substitute decimal points for commata
	boost::replace_all(text, ',', '.');
	
	//delete the selection early in case it contains a dot
	delete_selection();
	
	bool dot = false;
	
	// check each char in text, do not allow multiple dots
	BOOST_FOREACH(char ch, text) {
		if (!isdigit(ch) && ( only_integer_ || (dot & (dot |= (ch != '.'))) )) {
			return;
		}
	}
	if (dot && (text_.text().find('.') != std::string::npos)) {
		return;
	}

	selection_start_ += text_.insert_text(selection_start_, text);

	update_canvas();
	set_is_dirty(true);
	fire(event::NOTIFY_MODIFIED, *this, NULL);
}


void tnumbox::paste_selection(const bool mouse)
{
	const std::string& text = copy_from_clipboard(mouse);
	
	// trim leading and trailing whitespaces
	boost::algorithm::trim(text);
	// is something left?
	if(text.empty()) return;
	std::string restore = text_.text();
	delete_selection();
	selection_start_ += text_.insert_text(selection_start_, text);
	try {
		if (only_integer_) {
			int_val = boost::lexical_cast<int>(text_.text());
		} else {
			double_val = boost::lexical_cast<double>(text_.text());
	catch (boost::bad_lexical_cast) {
		text_.text() = restore;
		return;
	}
	update_canvas();
	set_is_dirty(true);
	fire(event::NOTIFY_MODIFIED, *this, NULL);
}