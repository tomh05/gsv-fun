#!/usr/bin/python
# coding: utf8

import sys
import hashlib
import urllib
import hmac
import base64
import urlparse


def googleUrlSign(urlString,privateKey):
    # Collect the URL input string (which must alrady be URL-encoded)
    # from the user and parse it into its collective components

    url = urlparse.urlparse(urlString)

    #We only need to sign the path+query part of the string
    urlToSign = url.path + "?" + url.query

    # Decode the private key into its binary format
    # We need toe decode the URL-encoded private key
    decodedKey = base64.urlsafe_b64decode(privateKey)

    # Create a signature using the private key and the URL-encoded
    # string using HMAC SHA1. This signature will be binary.
    signature = hmac.new(decodedKey, urlToSign, hashlib.sha1)

    # Encode the binary signature into base64 for use within a URL
    encodedSignature = base64.urlsafe_b64encode(signature.digest())
    originalUrl = url.scheme + "://" + url.netloc + url.path + "?" + url.query
    fullUrl = originalUrl + "&signature=" + encodedSignature
    return fullUrl

