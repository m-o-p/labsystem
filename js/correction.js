function startCorrection(elem) {
    elem.off('click');
    elem.attr('value', 'Save');
    window.onbeforeunload = function (e) {
        return discardChangesWarning;
    };
    // TODO: Transform div to textarea
    // - save original values as attributes
    // - LiElement.inc:269 (showComment)
    // - LiElement.inc:466 (showCredits)
    elem.click(function() {
        saveCorrection($(this));
    });
    elem.after('<input type="button" class="labsys_mop_i_cancel_button" value="Cancel" onclick="cancelCorrection(this)">');
}

function saveCorrection(elem) {
    toggleButton(elem);
    elem.next().remove();
    // TODO: Fire AJAX-request to save and warn on error!
    // - https://stackoverflow.com/questions/11082485/how-to-convert-simple-form-submit-to-ajax-call
    // - determine URL (we should not use "view all"-URL because of loading time!
    //  - probably introduce additional API?
    // - loading animation
    // - show updated data
    //  - status of second correction (icon LiElement:518ff & red/green color)
    //  - credits
    //  - comment(s)
}

function cancelCorrection(elem) {
    // TODO: Transform textarea to div
    // - Discard changes
    toggleButton($(elem).prev());
    $(elem).remove();
}

function toggleButton(elem) {
    elem.off('click');
    elem.attr('value', elem.attr('start-value'));
    window.onbeforeunload = null;
    elem.click(function() {
        startCorrection($(this));
    });
}

$(document).ready(function() {
    $('.labsys_tw_save_button').click(function() {
        $(this).attr('start-value', $(this).attr('value'));
        startCorrection($(this));
    });
});