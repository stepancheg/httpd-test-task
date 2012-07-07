#include <vector>

#include "misc.h"
#include "poller.h"

#include "reactor.h"


using namespace std;

struct reactor_descriptor::state : noncopyable {
    reactor* reactor_;
    shared_ptr<reactor_handler> handler_;
    class fd fd_;
    
    void change_mask(int mask);
};

struct reactor::state : noncopyable {
    reactor* reactor_;
    vector<shared_ptr<reactor_descriptor>> descriptors_;
    poller poller_;
    
    void add_descriptor(const fd& fd, shared_ptr<reactor_handler>& handler) {
        shared_ptr<reactor_descriptor> descriptor(new reactor_descriptor);
        descriptor->state_.reset(new reactor_descriptor::state);
        descriptor->state_->reactor_ = reactor_;
        descriptor->state_->handler_ = handler;
        descriptor->state_->fd_ = fd;
        descriptors_.push_back(descriptor);
        
        poller_.add_fd(fd, 0, descriptor.get());
        handler->initialize(descriptor.get());
    }
    
    void remove_descriptor(reactor_descriptor* descriptor) {
        for (size_t i = 0; i < descriptors_.size(); ++i) {
            if (descriptors_[i].get() == descriptor) {
                descriptors_.erase(descriptors_.begin() + i);
                return;
            }
        }
        proper_assert(false);
    }
    
    void run() {
        for (;;) {
            iteration();
        }
    }
    
    void iteration() {
        vector<poll_result> poll_results;
        poller_.poll(&poll_results);
        for (poll_result& result : poll_results) {
            auto descriptor = static_cast<reactor_descriptor*>(result.tag_);
            descriptor->state_->handler_->process_event();
        }
    }
    
    void debug_dump(ostream& os) {
        os << "descriptors:" << endl;
        for (auto descriptor : descriptors_) {
            os << "1" << endl;
        }
        os << "poller:" << endl;
        poller_.debug_dump(os);
    }
};

reactor::reactor(): state_(new state) {
    state_->reactor_ = this;
}

reactor::~reactor() {
}

void reactor::add_descriptor(const fd& fd, shared_ptr<reactor_handler>& handler) {
    state_->add_descriptor(fd, handler);
}

void reactor::run() {
    state_->run();
}

void reactor::debug_dump(ostream& os) {
    state_->debug_dump(os);
}

void reactor_descriptor::state::change_mask(int mask) {
    reactor_->state_->poller_.change_fd(fd_, mask);
}

void reactor_descriptor::change_mask(int mask) {
    state_->change_mask(mask);
}

const fd& reactor_descriptor::fd() {
    return state_->fd_;
}

void reactor_descriptor::remove() {
    state_->reactor_->state_->poller_.remove_fd(state_->fd_);
    state_->reactor_->state_->remove_descriptor(this);
}

// vim: set ts=4 sw=4 et:
