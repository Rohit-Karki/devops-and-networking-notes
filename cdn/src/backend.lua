local backend = {}
backend.generate_content = function ()
    -- ngx.header sets the current response header that is to be sent.
    ngx.header['content-type'] = 'application/json'
    -- we want the content to be cached by 10 seconds OR the provided max_age (ex: /path/to/service?max_age=40 for 40 seconds)
    ngx.header['cache-control'] = 'public, max-age=' .. (ngx.var.arg_max_age or 10)
    
    -- ngx.say will write the response body
    ngx.say('{"service": "api", "value": 42, "request": "'.. ngx.var.uri ..'"}')
end

return backend