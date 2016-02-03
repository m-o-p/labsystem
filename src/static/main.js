function registerAnswer(data) {
  var label = data.answer.children('div.answer-label');
  var answer = data.answer.children('form');

  function disable() {
    answer.prop('disabled', true);

    answer.find('button').prop('disabled', true);
    answer.find('textarea').prop('disabled', true);
  }

  function enable() {
    answer.prop('disabled', false);

    answer.find('button').prop('disabled', false);
    answer.find('textarea').prop('disabled', false);
  }

  $(function () {
    label.show();

    if (!data.locked) {
      disable();
    }

    label.click(function () {
      var isDisabled = answer.prop('disabled');

      if (isDisabled) {
        $.post(data.lockUrl)
          .done(function () {
            enable();
          });
      } else {
        $.post(data.unlockUrl)
          .done(function () {
            disable();
          });
      }
    });
  });
}
