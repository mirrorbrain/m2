import hashlib, random, string, time, argparse

# ------------------------------------------------------------------------------
# returns a random string with "length" characters
def get_random_string(length):
    return ''.join(random.choice(string.letters.join(string.digits)) for i in range(length))

# ------------------------------------------------------------------------------
# computes the sha512 for the given message and returns the first k bytes
def get_truncated_hash(message, k):
    return hashlib.sha512(message).digest()[-k:]

# ------------------------------------------------------------------------------
# helper method for pretty search result printing
def print_results(m0, m1, hash, k):
    print "Message A:", m0.encode("hex")   # encode to human-readable hex values
    print "Message B:", m1.encode("hex")
    print "Same hash:", hash.encode("hex")

# ------------------------------------------------------------------------------
# searches for k-byte collisions with the birthday attack. if a dp_pattern is
# provided, only hashes ending with those bytes will be considered
def simple_birthday(k):
    hash_table  = dict()

    while True:
        s    = get_random_string(k)
        hash = get_truncated_hash(s, k)

        if hash in hash_table:
            if hash_table[hash] != s:
                print_results(s, hash_table[hash], hash, k)
                break
        else:
            hash_table[hash] = s

def collision_no_memory(k):
    x = y = get_random_string(k)
    x1 = get_truncated_hash(x, k)
    x2 = get_truncated_hash(x1, k)
    y1 = get_truncated_hash(get_truncated_hash(y, k), k)
    y2 = get_truncated_hash(get_truncated_hash(y1, k), k)

    while x2 != y2:
        x = x1
        x1 = x2
        x2 = get_truncated_hash(x2, k)
        y1 = y2
        y2 = get_truncated_hash(get_truncated_hash(y2, k), k)

    if x1 != get_truncated_hash(y1, k):
        print_results()

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    # add command line options
    parser = argparse.ArgumentParser(description='Search for near collisions of sha512.')
    parser.add_argument('-k', metavar="BYTES", default=4, type=int, help='Amount of bytes which shall collide. Use a value of -1 to start a infinite search for a large k.')
    args = parser.parse_args()

    # search mode
    if args.k == -1:
        k = 2
        while True:
            print "Searching a collision for k =",k,"..."
            simple_birthday(k)
            k += 1
    # profile mode
    else:
        simple_birthday(args.k)
