$('a.confirmDelete').click(function(el) {
  el.link += '&isConfirmed=1';
  return confirm(t("confirmDelete"));
});

$('div.hiddenDiv > a.hiddenDivLink').click(function(el) {
  el.siblings('div.hiddendivContents').style('display', 'block');

  return false;
});

$('div.hiddenDiv > div.hiddendivContents').prepend($('div.hiddenDivCloseButton'));

$('div.hiddenDiv > div.hiddendivContents > div.hiddenDivCloseButton > a').click(function(el) {
  el.parent().parent().style('display', 'none');

  return false;
});

$('div.clickSelect').click(function(el) {
  el.select();
});
