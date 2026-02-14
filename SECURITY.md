# Security Policy

## Supported Versions

Currently supported versions with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | :white_check_mark: |

## Reporting a Vulnerability

We take security seriously. If you discover a security vulnerability, please follow these steps:

### What to Report

Please report any issues that could affect:
- System security
- Data integrity
- User privacy
- Denial of service vulnerabilities
- Code execution vulnerabilities
- Information disclosure

### How to Report

**DO NOT** open a public issue for security vulnerabilities.

Instead:
1. Email the security concern to the project maintainer
2. Include "SECURITY" in the subject line
3. Provide a detailed description of the vulnerability
4. Include steps to reproduce the issue
5. Suggest a fix if possible

### What to Expect

- **Acknowledgment**: Within 48 hours
- **Initial Assessment**: Within 1 week
- **Status Updates**: Every 2 weeks until resolved
- **Fix Timeline**: Depends on severity:
  - Critical: 1-7 days
  - High: 1-2 weeks
  - Medium: 2-4 weeks
  - Low: Best effort

### After Reporting

Once the vulnerability is fixed:
1. We'll notify you when a fix is available
2. Credit will be given (if desired)
3. A security advisory will be published
4. Users will be notified to update

## Security Best Practices

When using this software:

### General Security

1. **Keep Software Updated**
   - Always use the latest version
   - Check for security updates regularly
   - Subscribe to release notifications

2. **Run with Minimal Privileges**
   ```bash
   # Don't run as root unless necessary
   ./stress_test
   
   # For system-wide installation, only install as root
   sudo make install
   ```

3. **Network Security**
   - By default, the server binds to localhost only
   - Be careful when exposing to network
   - Use firewall rules to restrict access
   ```bash
   # Bind to localhost only (default)
   ./stress_test 8080
   
   # If exposing to network, use firewall
   sudo ufw allow from 192.168.1.0/24 to any port 8080
   ```

4. **Monitor Resource Usage**
   - Keep an eye on system resources during tests
   - Set reasonable test durations
   - Use auto-stop on overheat (when implemented)

### For Developers

If you're modifying the code:

1. **Input Validation**
   - Always validate user input
   - Sanitize data from web requests
   - Check bounds for numerical inputs

2. **Memory Safety**
   - Use smart pointers where appropriate
   - Avoid buffer overflows
   - Check for memory leaks with valgrind
   ```bash
   valgrind --leak-check=full ./stress_test
   ```

3. **Thread Safety**
   - Use mutexes for shared data
   - Avoid race conditions
   - Test with thread sanitizer
   ```bash
   g++ -fsanitize=thread -g -o stress_test src/*.cpp
   ```

4. **Error Handling**
   - Never ignore errors
   - Fail securely
   - Log security-relevant events

## Known Security Considerations

### Temperature Monitoring
- Reading temperature sensors requires permissions
- On Linux: May need access to `/sys/class/thermal/`
- Solution: Run with appropriate permissions or disable monitoring

### Network Exposure
- The embedded HTTP server is minimal
- Not hardened for internet exposure
- **Recommendation**: Use behind reverse proxy if internet-facing
- Example with nginx:
  ```nginx
  location /stress-test/ {
      proxy_pass http://localhost:8080/;
      proxy_set_header Host $host;
      proxy_set_header X-Real-IP $remote_addr;
  }
  ```

### Resource Exhaustion
- Stress tests intentionally use 100% of resources
- This is normal and expected behavior
- However, this could impact other services
- **Recommendation**: Run on dedicated test machines when possible

### Privilege Escalation
- Installation script requires root privileges
- Review scripts before running with sudo
- Install from trusted sources only

## Disclosure Policy

We follow coordinated disclosure:

1. **Private Notification**
   - Reporter is notified first
   - Details are kept confidential during fix

2. **Patch Development**
   - Fix is developed privately
   - Security implications are assessed

3. **Public Disclosure**
   - After fix is released and tested
   - Advisory is published with credit
   - Users are notified to update

4. **Timeline**
   - 90 days maximum before public disclosure
   - Can be extended if actively being fixed
   - Shortened for active exploits

## Security Features

Current security measures:

- ✅ Input validation on API endpoints
- ✅ No hardcoded credentials
- ✅ Minimal attack surface
- ✅ Thread-safe operations
- ✅ Memory safety (RAII, smart pointers where used)
- ✅ No shell injection vulnerabilities
- ✅ CORS headers (configurable)

Planned security improvements:

- [ ] Rate limiting on API endpoints
- [ ] Authentication/authorization options
- [ ] HTTPS support
- [ ] Request logging
- [ ] Security headers (CSP, etc.)
- [ ] Input sanitization hardening

## Dependencies

This project has minimal dependencies:
- C++ Standard Library (std)
- pthread (POSIX threads)
- System libraries for hardware monitoring

No third-party libraries with known vulnerabilities.

## Compliance

This software:
- Does NOT collect user data
- Does NOT send telemetry
- Does NOT require registration
- Does NOT access the internet (except local network if configured)
- Does NOT modify system files (except during installation)

## License

Security vulnerabilities and fixes are covered under the same MIT License as the project.

---

**Last Updated**: 2026-02-14

**Contact**: See project repository for contact information

**MIT License © 2026 kj-devvixon**
