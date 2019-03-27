#include "utils.hpp"
#include <string.h>

enum _boundry_type {
    BOUNDRY_NO = 0,
    BOUNDRY_FIRST,
    BOUNDRY_MIDDLE,
    BOUNDRY_LAST,
    BOUNDRY_BAK,
};
class BoundryItem
{
    public:
        bool _is_download;
        std::unordered_map<std::string, std::string> _hdr_list;
        std::unordered_map<std::string, std::string> _dps_list;
    public:
        BoundryItem():_is_download(false) {}
        bool IsFileDownLoad() {
            if (_dps_list.find("filename") != _dps_list.end()) {
                return true;
            }
            return false;
        }
        bool GetField(const std::string &key, std::string &val) {
            auto it = _dps_list.find(key);
            if (it == _dps_list.end()) {
                return false;
            }
            val = it->second; 
            return true;
        }
        bool GetFileName(std::string &name) {
            return GetField("filename", name);
        }
};
class UpLoad
{
    private:
        int64_t _cont_len;
        int64_t _curt_len;
        int64_t _buf_clen;
        char _tmp_buf[MAX_BUFF];
        std::string _f_boundry;
        std::string _m_boundry;
        std::string _l_boundry;
    public:
        UpLoad():_cont_len(0),_buf_clen(0),_curt_len(0) {}
        bool InitUpLoadInfo() {
            //初始化文件上传信息 Content-Length Query-String Boundry
            umask(0);
            char *ptr = NULL;
            if ((ptr = getenv("Content-Length")) == NULL) {
                LOG("have no content length\n");
                return false;
            }
            _cont_len = Utils::StrToDigit(ptr);
            if ((ptr = getenv("Content-Type")) == NULL) {
                LOG("have no content type\n");
                return false;
            }
            std::string content_type = getenv("Content-Type");

            std::string boundary_seg = "boundary=";
            size_t pos = content_type.find(boundary_seg);
            if (pos == std::string::npos) {
                LOG( "have no boundry\n");
                return false;
            }
            std::string boundry = content_type.substr(pos + boundary_seg.length()) ;
            _f_boundry = "--" + boundry;
            _m_boundry = "\r\n--" + boundry + "\r\n";
            _l_boundry = "\r\n--" + boundry + "--";
            return true;
        }
        bool ReadContent() {
            if (_curt_len >= _cont_len) {
                return false;
            }
            int rlen = (MAX_BUFF - _buf_clen) > (_cont_len - _curt_len) ? \
                       (_cont_len - _curt_len) : (MAX_BUFF - _buf_clen);
            int ret = read(0, _tmp_buf + _buf_clen, rlen);
            if (ret <= 0) {
                return false;
            }
            _buf_clen += ret;
            _curt_len += ret;
            return true;
        }
        bool MatchBoundry(int *pos) {
            if (!memcmp(_tmp_buf, _f_boundry.c_str(), _f_boundry.length())) {
                *pos = 0;
                LOG("MATCH first boundry\n");
                return true;
            }
            for (int i = 0; i < _buf_clen; i++) {
                if ((_buf_clen - i) < _l_boundry.length()) {
                    *pos = i;
                    break;
                }
                if (!memcmp(_tmp_buf + i, _m_boundry.c_str(), _m_boundry.length())) {
                    *pos = i;
                    LOG("MATCH middle boundry\n");
                    return true;
                }
                if (!memcmp(_tmp_buf + i, _l_boundry.c_str(), _l_boundry.length())) {
                    *pos = i;
                    LOG("MATCH last boundry\n");
                    return true;
                }
            }
            return false;
        }
        bool GetBoundry(BoundryItem *item) {
            ReadContent();
            char *ptr = strstr(_tmp_buf, "\r\n\r\n");
            if (ptr == NULL) {
                return false;
            }
            std::string bhdr;
            bhdr.assign(_tmp_buf, ptr - _tmp_buf);
            _buf_clen -= (ptr + 4 - _tmp_buf);
            memmove(_tmp_buf, ptr + 4, _buf_clen);
            LOG("boundry:[%s]\n\n", bhdr.c_str());
            
            std::vector<std::string> hdr_list;
            Utils::Split(bhdr, "\r\n", hdr_list);
            for (int i = 0; i < hdr_list.size(); i++) {
                size_t pos = hdr_list[i].find(": ");
                item->_hdr_list[hdr_list[i].substr(0, pos)] = hdr_list[i].substr(pos + 2);
            }
            
            auto it = item->_hdr_list.find("Content-Disposition");
            if (it == item->_hdr_list.end()) {
                LOG("Get Boundry Failed:%s\n", bhdr.c_str());
                return false;
            }
            std::vector<std::string> pos_list;
            Utils::Split(it->second, "; ", pos_list);
            for (int i = 0; i < pos_list.size(); i++) {
                size_t pos = pos_list[i].find("=");
                std::string file = pos_list[i].substr(pos + 1);
                std::string tmp;
                for (int j = 0; j < file.length(); j++) {
                    if (file[j] == '\"') {
                        continue;
                    }
                    tmp.push_back(file[j]);
                }
                item->_dps_list[pos_list[i].substr(0, pos)] = tmp;
            }
            return true;
        }
        bool StoreFile(const std::string &path, std::string &name) {
            if (access(path.c_str(), F_OK) < 0) {
                mkdir(path.c_str(), 0775);
            }
            std::string file = path + name;
            int fd = open(file.c_str(), O_CREAT|O_WRONLY, 0664);
            if (fd < 0) {
                LOG("open file error:%s\n", strerror(errno));
                return false;
            }
            while(1) {
                ReadContent();
                int pos;
                if (MatchBoundry(&pos)) {
                    WriteFile(fd, pos);
                    break; 
                }else {
                    WriteFile(fd, pos);
                }
            }
            close(fd);
            return true;
        }
        bool WriteFile(int fd, int len) {
            if (write(fd, _tmp_buf, len) < 0) {
                LOG("write error:%s\n", strerror(errno));
            }
            _buf_clen -= len;
            memmove(_tmp_buf, _tmp_buf + len, _buf_clen);
            return true;
        }
        bool DropData() {
            while(1) {
                ReadContent();
                int pos;
                if (MatchBoundry(&pos)) {
                    _buf_clen -= pos;
                    memmove(_tmp_buf, _tmp_buf + pos, _buf_clen);
                    break; 
                }else {
                    _buf_clen -= pos;
                    memmove(_tmp_buf, _tmp_buf + pos, _buf_clen);
                }
            }
            return true;
        }
        bool BoundaryOver() {
            if (!memcmp(_tmp_buf, _l_boundry.c_str(), _l_boundry.length())) {
                return true;
            }
            return false;
        }
        bool ProcessUpLoad() {
            while(1) {
                if (BoundaryOver()) {
                    return true;
                }
                BoundryItem it;
                CHECK_RET(GetBoundry(&it));
                if (it.IsFileDownLoad()) {
                    std::string name;
                    if (it.GetFileName(name) && !StoreFile("./www/", name)) {
                        return false;
                    }
                }
                DropData();
            }
            return true;
        }
};

int main()
{
    UpLoad upload;
    CHECK_RET(upload.InitUpLoadInfo());
    if (upload.ProcessUpLoad()) {
        std::cout<<"<html><body><h1>UPLOAD SUCCESS!!</h1></body></html>";
    }else {
        std::cout<<"<html><body><h1>UPLOAD FAILED!!</h1></body></html>";
    }
    fflush(stdout);
    return 0;
}
