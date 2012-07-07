#include <poll.h>

#include <vector>

#include "misc.h"
#include "backtrace.h"

#include "poller.h"

using namespace std;


static int translate_poller_mask_to_poll_mask(int poller_mask) {
    int poll_mask = 0;
    if (poller_mask & POLLER_READ) {
        poll_mask |= POLLIN;
    }
    if (poller_mask & POLLER_WRITE) {
        poll_mask |= POLLOUT;
    }
    return poll_mask;
}

struct poller::state {
    struct request {
        int fd_;
        int mask_;
        void* tag_;
    };

    vector<pollfd> pollfds_;
    vector<request> requests_;
    
    void add_fd(int fd, int mask, void* tag) {
        request r;
        r.fd_ = fd;
        r.mask_ = mask;
        r.tag_ = tag;
        
        pollfd pfd;
        pfd.fd = fd;
        pfd.events = translate_poller_mask_to_poll_mask(mask);
        
        requests_.push_back(r);
        pollfds_.push_back(pfd);
    }
    
    size_t index_of_fd(int fd) {
        proper_assert(fd >= 0);
        for (size_t i = 0; i < requests_.size(); ++i) {
            if (requests_[i].fd_ == fd) {
                return i;
            }
        }
        proper_assert(false);
    }
    
    void remove_fd(int fd) {
        size_t i = index_of_fd(fd);
        requests_.erase(requests_.begin() + i);
        pollfds_.erase(pollfds_.begin() + i);
    }
    
    void change_fd(int fd, int mask) {
        size_t i = index_of_fd(fd);
        requests_[i].mask_ = mask;
        pollfds_[i].events = translate_poller_mask_to_poll_mask(mask);
    }

    void poll(vector<poll_result>* result) {
        result->clear();
        proper_assert(pollfds_.size() > 0);
        for (pollfd& pfd : pollfds_) {
            pfd.revents = 0;
        }
        // TODO: also implement with epoll
        int r = ::poll(&pollfds_[0], pollfds_.size(), -1);
        proper_assert(r > 0); // TODO, it maybe just sleep
        for (size_t i = 0; i < pollfds_.size(); ++i) {
            pollfd& pfd = pollfds_[i];
            request& req = requests_[i];
            if (pfd.revents != 0) {
                poll_result r;
                r.fd_ = req.fd_;
                r.tag_ = req.tag_;
                result->push_back(r);
            }
        }
    }
    
    void debug_dump(ostream& os) {
        for (auto pfd : pollfds_) {
            os << pfd.fd << " " << pfd.events << endl;
        }
    }
};

poller::poller(): state_(new state) {
}

poller::~poller() {
}

void poller::poll(vector<poll_result>* result) {
    state_->poll(result);
}

void poller::add_fd(int fd, int mask, void* tag) {
    state_->add_fd(fd, mask, tag);
}

void poller::change_fd(int fd, int mask) {
    state_->change_fd(fd, mask);
}

void poller::remove_fd(int fd) {
    state_->remove_fd(fd);
}

void poller::debug_dump(ostream& os) {
    state_->debug_dump(os);
}


#include <fcntl.h>
#include <unistd.h>

#include "test.h"

void test_poller_simple() {
    int fd = open("/etc/passwd", O_RDONLY);
    ua(fd >= 0);
    poller p;
    p.add_fd(fd, POLLER_READ, 0);
    vector<poll_result> r;
    p.poll(&r);
    ua_eq(1, r.size());
    p.poll(&r);
    ua_eq(1, r.size());
    p.remove_fd(fd);
    close(fd);
}

void test_translate_poller_mask_to_poll_mask() {
    ua_eq(POLLIN, translate_poller_mask_to_poll_mask(POLLER_READ));
    ua_eq(POLLOUT, translate_poller_mask_to_poll_mask(POLLER_WRITE));
    ua_eq(POLLIN|POLLOUT, translate_poller_mask_to_poll_mask(POLLER_READ|POLLER_WRITE));
}

// vim: set ts=4 sw=4 et:
