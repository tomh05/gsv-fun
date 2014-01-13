#!/usr/bin/python
# coding: utf8

import sys
import hashlib
import urllib
import hmac
import base64
import urlparse


def googleUrlSign(inputStr):
    # Collect the URL input string (which must alrady be URL-encoded)
    # from the user and parse it into its collective components
    #inputStr = "http://maps.googleapis.com/maps/api/streetview?size=600x400&location=40.720032,-73.988354&sensor=false&client=gme-britishbroadcasting"

    url = urlparse.urlparse(inputStr)

    # Collect the private key. You may want to store this
    # in a private file.
    privateKey = "ZFb0O2mmKm_HXX6Fi4vYIUj6SQU="



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

