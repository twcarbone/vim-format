let heredoc_items =<< trim END
first line that is very long to intentionally exceed the testing column limit... more text
    one
        END

        {a + b}

END

let heredoc_eval_items =<< eval END
{&tpm + 2} is 12
    END {c} is a list
    echo {@@}
END
