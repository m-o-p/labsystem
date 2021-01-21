/**
 * This variable is used for the dirty bit functionality.
 * The dirty bit is set by the inputs when they are changed.
 * It is cleared by the save buttons.
 * If not cleared it shows a warning.
 */
var isDirty = false;
var retVal = null;

var body = $('body');

function dirtyWarning() {
    if (isDirty) return confirm(discardChangesWarning);
    return retVal;
}


window.addEventListener('beforeunload', function (event) {
    if (dirtyWarning()) {
        event.preventDefault();
        event.returnValue = '';
    }
});

if (body.data('enable_tinymce') === 1) {
    // Called when content is loaded from textarea
    // Remove beginning [HTML]-tag of labsystem fields
    function myCustomSetupContent(editor_id, body, doc) {
        if (body.innerHTML.substring(0, 6) === "[HTML]") {
            body.innerHTML = body.innerHTML.substring(6);
        }
    }

    // Custom save callback, gets called when the contents is to be submitted
    // Add beginning [HTML]-tag to labsystem fields
    function myCustomSave(id, content) {
        // gets somehow called twice...
        if (content.substring(0, 6) !== "[HTML]") {
            content = "[HTML]\n" + content;
        }
        return content;
    }

    tinyMCE.init({
        // only fields called "CONTENTS" should get TinyMCEed
        mode: "exact",
        elements: "CONTENTS",
        // Should the user be asked if TinyMCE should get started? (onClick)
        ask: false,
        setupcontent_callback: "myCustomSetupContent",
        save_callback: "myCustomSave",
        // Labsystems stylesheet to format the content correctly
        content_css: body.data('usercss'),
        theme: "advanced",
        // hinders tinyMCE from putting <p> around [HTML]
        forced_root_block: ""
    });
}

// CodeMirror
$(".codemirror_elt").each(function () {
    var cm_target = $(this).find('.codemirror_target');
    if (cm_target.length > 0) {
        var preview_target = $("#" + cm_target.data('previewtarget'));
        var cm = CodeMirror.fromTextArea(cm_target[0], {
            lineNumbers: true,
            lineWrapping: true,
            autoCloseTags: true,
            extraKeys: {"Ctrl-Space": "autocomplete"},
            foldGutter: true,
            gutters: [ "CodeMirror-foldgutter", "CodeMirror-linenumbers"],
            matchBrackets: true,
            matchTags: true,
            mode: {
                name: "text/html",
                htmlMode: true,
            }
        });

        $(this).find('a.refresh_codemirror[data-toggle="tab"]').on('shown.bs.tab', function () {
            cm.refresh();
        });
        $(this).find('a.previewtab[data-toggle="tab"]').on('shown.bs.tab', function () {
            preview_target.html(cm.getValue());
        });
    }
});