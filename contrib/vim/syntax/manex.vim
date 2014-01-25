syn match manexError   "^[^ ].*"
syn match manexName    "\%^.*"
syn match manexExample "^example"
syn match manexText    "^text"
syn match manexCode    "^code"    nextgroup=manexCC skipnl
syn match manexCC      "^ .*"     nextgroup=manexCC skipnl contained

hi def link manexError   Error
hi def link manexName    Type
hi def link manexExample NonText
hi def link manexText    Statement
hi def link manexCode    Statement
hi def link manexCC      Constant
